#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>			// access()
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>


#define HASH_FILE_PATH "/home/grasslog/tmp/IP/ip_data.dat"
#define HASH_GENERATION_PATH "/home/grasslog/tmp/hashfile"
#define BUFFER_SIZE 20

const int num = 100000000;		// url number  
const int TOP_URL_NUMBER = 100;
const int HASH_FILE_NUMBER = 1000;

node top_heap[TOP_URL_NUMBER];
bool is_top_heap = false;

unsigned int iton(char *ip);
void ntoi(unsigned int num, char *ip);
int  fileexist(char *path);			// judge file exit
void fclose_all(FILE **t);			// close all files
int  random_write(const char *path);		// random generation url address
// calculate top IP address
void count(char *hashfile, unsigned int *data, unsigned int *num);
void sort(unsigned int *max, unsigned int *ip, int n);		// sort url
inline unsigned int hash(unsigned int ip)				// hash function
{ return (ip % HASH_FILE_NUMBER); }
void swap(node &a, node &b);		// local swap struct node function

typedef struct node		// storage struct node
{
	std::string ip;	// IP
	unsigned int n;		// occurrence number
	node(std::string _ip, unsigned int _n):ip(_ip),n(_n) {}
	node():ip(""),n(0) {}
}node;

// min root heap
void insert_heap(node *a, int n, int m);
void make_heap(node *a, int m);

int main(void)
{
	FILE *in = NULL;
	FILE *tmpfile[HASH_FILE_NUMBER];
	const char *path = HASH_FILE_PATH;
	char hashfile[HASH_FILE_NUMBER];
	char buf[BUFFER_SIZE];
	unsigned int add, data, n;
	unsigned int ip[TOP_URL_NUMBER], max[TOP_URL_NUMBER];	// top 10 IP
	unsigned int t1, t2, s, e;		// record the time
	int i, j, len, now;			// IP number
	node top_heap[TOP_URL_NUMBER];
	bool is_top_heap = false;

	printf("Generating data %s\n\n", path);
	if (!random_write(path)) return 0;	// random generation IP log file

	// judge file exit, access() == 0 exit
	if (access(HASH_GENERATION_PATH, 0) == 0) 
		system("rm -r HASH_GENERATION_PATH");
	system("mkdir HASH_GENERATION_PATH");		// mkdir  /home/grasslog/tmp/hashfile working drectory
	//system("attrib +h /home/grasslog/tmp/hashfile");

	in = fopen(path, "rt");			// open IP log file
	if (in == NULL) return 0;
	for (i=0; i<TOP_URL_NUMBER; i++) tmpfile[i] = NULL;

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
	for (i=0; i<HASH_FILE_NUMBER; i++)
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

	for (i=0; i<TOP_URL_NUMBER; i++)
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
		for (j=0; j<BUFFER_SIZE; j++) buf[j] = '\0';
		cur = buf;
		for (j=0; j<4; j++) 
		{
			b = rand() % 256;
			sprintf(cur, "%d.", b);
			while (*cur != '\0') cur++;
		}
		*(cur - 1) = '\n';
		fwrite(buf, cur-(char *)buf, 1, out);
	}
	fclose(out);		// close IO stream
	return 1;
}

// calculate top IP in hashfile
void count(char *hashfile)
{
	FILE *in = NULL;
	std::string ip;
	std::unordered_map<std::string,int> ump;

	in = fopen(hashfile, "rt");
	while(fscanf(in, "%s", &ip) != EOF)
	{
		ump[ip]++;
	}
	std::vector<node> vec_node;
	typedef std::unordered_map<std::string,int>::iterator ump_iterator;

	for(ump_iterator it=ump.begin(); it!=ump.end(); ++it)
	{
		node t(it->first,it->second);
		vec_node.push_back(t);
	}
	fclose(in);
	if(!is_top_heap)
	{
		is_top_heap = !is_top_heap;
		memcpy(top_heap,&vec_node,sizeof(struct node)*TOP_URL_NUMBER);
		make_heap(top_heap,TOP_URL_NUMBER);
		for(int i=100; i<vec_node.size(); i++)
		{
			node t = vec_node[i];
			int w = top_heap[0].n;

			if(t.n > w)
			{
				swap(t,top_heap[0]);	// local swap function
			}
			insert_heap(top_heap,0,TOP_URL_NUMBER);
		}
	}
	else
	{
		for(int i=0; i<vec_node.size(); i++)
		{
			node t = vec_node[i];
			int w = top_heap[0].n;

			if(t.n > w)
			{
				swap(t, top_heap[0]);
			}
			insert_heap(top_heap,0,TOP_URL_NUMBER);
		}
	}
	
}

void insert_heap(node *heap, int n, int m)
{
	node t = heap[n];
	int w = t.n;
	int i = n;
	int j = 2 * i + 1;
	
	while(j < m)
	{
		if(j+1<m && heap[j+1].n<heap[j].n)
			++j;

		if(heap[j].n < w)
			heap[i] = heap[j];
		else break;

		i = j;
		j = 2 * i + 1;
	}
	
	heap[j] = t;
}

void make_heap(node *heap, int m)
{
	for(int i=m/2; i>=0; i--)
	{
		insert_heap(heap,i,m);
	}
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
