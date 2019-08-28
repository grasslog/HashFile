#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>			// access()
#include <string.h>

const int num = 100000000;		// url number  about 1GB

unsigned int iton(char *ip);
void ntoi(unsigned int num, char *ip);
int  fileexist(char *path);			// judge file exit
void fclose_all(FILE **t);			// close all files
int  random_write(const char *path);		// random generation url address
// calculate top IP address
void count(char *hashfile, unsigned int *data, unsigned int *num);
void sort(unsigned int *max, unsigned int *ip, int n);		// sort url
inline unsigned int hash(unsigned int ip)				// hash function
{ return (ip % 1000); }

typedef struct node		// binary tree node
{
	unsigned int ip;	// IP
	unsigned int n;		// occurrence number
	node *left;
	node *right;
}node;

int main(void)
{
	FILE *in = NULL;
	FILE *tmpfile[1005];
	const char *path = "/home/grasslog/tmp/IP/ip_data.dat";
	char hashfile[50];
	char buf[20];
	unsigned int add, data, n;
	unsigned int ip[10], max[10];	// top 10 IP
	unsigned int t1, t2, s, e;		// record the time
	int i, j, len, now;			// IP number

	printf("Generating data %s\n\n", path);
	if (!random_write(path)) return 0;	// random generation IP log file

	// judge file exit, access() == 0 exit
	if (access("/home/grasslog/tmp/hashfile", 0) == 0) 
		system("rm -r /home/grasslog/tmp/hashfile");
	system("mkdir /home/grasslog/tmp/hashfile");		// mkdir  /home/grasslog/tmp/hashfile working drectory
	//system("attrib +h /home/grasslog/tmp/hashfile");

	in = fopen(path, "rt");			// open IP log file
	if (in == NULL) return 0;
	for (i=0; i<1005; i++) tmpfile[i] = NULL;

	// make 1000000000 IP hash in 1005 small files
	printf("\r hashing %s\n\n", "/home/grasslog/tmp/hashfile");
	e = s = t1 = clock();		// start time
	now = 0;
	while (fscanf(in, "%s", buf) != EOF)
	{
		data = iton(buf);		// IP digitization
		add = hash(data);		// math hash address
		sprintf(hashfile, "/home/grasslog/tmp/hashfile/hashfile_%u.dat", add);
		if (tmpfile[add] == NULL)
			tmpfile[add] = fopen(hashfile, "a");
		sprintf(buf, "%u\n", data);
		len = strlen(buf);
		// write IP in files, I/O just be the bottlenneck
		fwrite(buf, len, 1, tmpfile[add]);
		now++;
		e = clock();
		if (e - s > 1000)		// calculate rate of progress
		{
			printf("\rProcessing progress %0.2f %%\t", (now * 100.0) / num);
			s = e;
		}
	}
	fclose(in);
	fclose_all(tmpfile);
	remove(path);

	// calculate top IP in each small files
	for (i=0; i<10; i++) max[i] = 0;
	for (i=0; i<1000; i++)
	{
		sprintf(hashfile, "/home/grasslog/tmp/hashfile/hashfile_%d.dat", i);
		if (fileexist(hashfile))
		{
			printf("\rProcessing hashfile_%d.dat\t", i);
			count(hashfile, &data, &n);
			unsigned int min = 0xFFFFFFFF, pos;
			for (j=0; j<10; j++)
			{
				if (max[j] < min)
				{
					min = max[j];
					pos = j;
				}
			}
			if (n > min)
			{
				max[pos] = n;
				ip[pos] = data;
			}
		}
	}
	t2 = clock();		// end time
	sort(max, ip, 10);

	FILE *log = NULL;		// record in /home/grasslog/tmp/hashfile/ip_result.txt
	log = fopen("/home/grasslog/tmp/hashfile/ip_result.txt", "wt");
	fprintf(log, "\ntop 10 IP:\n\n");
	fprintf(log, "     %-15s%s\n", "IP", "Visits");
	printf("\n\ntop 10 IP:\n\n");
	printf("     %-15s%s\n", "IP", "Visits");
	for (i=0; i<10; i++)
	{
		ntoi(ip[i], buf);		// decord
		printf("   %-20s%u\n", buf, max[i]);
		fprintf(log, "   %-20s%u\n", buf, max[i]);
	}
	fprintf(log, "\n--- spent %0.3f second\n", (t2 - t1) / 1000.0);
	printf("\n--- spent %0.3f second\n\n", (t2 - t1) / 1000.0);
	fclose(log);

	return 0;
}

void fclose_all(FILE **t)		// close all files
{
	int i;

	for (i=0; i<1000; i++)
	{
		if (t[i])
		{
			fclose(t[i]);
			t[i] = NULL;
		}
	}
}

// random generation url address
int random_write(const char *path)
{
	FILE *out = NULL;
	int i, j, b;
	char buf[20];
	char *cur;
	unsigned int s, e;

	out = fopen(path, "wt");
	if (out == NULL) return 0;
	srand(time(NULL));
	e = s = clock();
	for (i=0; i<num; i++)
	{
		e = clock();
		if (e - s > 1000)		// calculate rate of progress
		{
			printf("\rProcessing progress %0.2f %%\t", (i * 100.0) / num);
			s = e;
		}
		for (j=0; j<20; j++) buf[j] = '\0';
		cur = buf;
		for (j=0; j<4; j++) 
		{
			b = rand() % 255;
			sprintf(cur, "%d.", b);
			while (*cur != '\0') cur++;
		}
		*(cur - 1) = '\n';
		fwrite(buf, cur-(char *)buf, 1, out);
	}
	fclose(out);		// close IO stream
	return 1;
}

// insert binary tree
void insert(node **tree, unsigned int ip)
{
	if ((*tree) == NULL)
	{
		// new_node
		(*tree) = (node *)malloc(sizeof(node));
		(*tree)->ip = ip;
		(*tree)->n = 1;
		(*tree)->left = (*tree)->right = NULL;
	}
	else if ((*tree)->ip == ip)
	{
		(*tree)->n++;
		return ;
	}
	else if (ip < (*tree)->ip)		// the left child tree
		insert(&((*tree)->left), ip);
	else insert(&((*tree)->right), ip);	// the right child tree
}

unsigned int maxn;		
node *max_node;			
void max_n(node *tree)	// find top node
{
	if (tree)
	{
		if (tree->n > maxn)
		{
			maxn = tree->n;
			max_node = tree;
		}
		max_n(tree->left);
		max_n(tree->right);
	}
}

void destory(node *tree)		// free tree node space
{
	if (tree)
	{
		destory(tree->left);
		destory(tree->right);
		free(tree);
	}
}

// calculate top IP in hashfile
void count(char *hashfile, unsigned int *data, unsigned int *n)
{
	FILE *in = NULL;
	node *tree = NULL;
	unsigned int ip;

	in = fopen(hashfile, "rt");
	while (fscanf(in, "%d", &ip) != EOF)
	{
		insert(&tree, ip);
	}
	fclose(in);
	maxn = 0;
	max_n(tree);
	*n = max_node->n;
	*data = max_node->ip;
	destory(tree);
}

// sort algorithm
void sort(unsigned int *max, unsigned int *ip, int n)
{
	int i, j;
	unsigned int tmpm, tmpi;

	for (i=1; i<n; i++)
	{
		if (max[i-1] < max[i])
		{
			tmpm = max[i];
			tmpi = ip[i];
			for (j=i; j>0; j--)
			{
				if (max[j-1] < tmpm)
				{
					max[j] = max[j-1];
					ip[j]  = ip[j-1];
				}
				else break;
			}
			max[j] = tmpm;
			ip[j]  = tmpi;
		}
	}
}

// IP to int
unsigned int iton(char *ip)
{
	unsigned int r = 0;
	unsigned int t;
	int i, j;

	for (j=i=0; i<4; i++)
	{
		sscanf(ip + j, "%d", &t);
		if (i < 3)
			while (ip[j++] != '.');
		r = r << 8;
		r += t;
	}
	return r;
}

// num to IP
void ntoi(unsigned int num, char *ip)
{
	unsigned int b, f;
	int i, cur;

	f = 0x00FFFFFF;
	cur = 0;
	for (i=3; i>=0; i--)
	{
		b = num >> (i * 8);
		num = num & f;
		f = f >> 8;
		sprintf(ip + cur, "%u.", b);
		while (ip[cur] != '\0') cur++;
	}
	ip[cur - 1] = '\0';
}

// judge file exist
int fileexist(char *path)
{
	FILE *fp = NULL;

	fp = fopen(path, "rt");
	if (fp)
	{
		fclose(fp);
		return 1;
	}
	else return 0;
}
