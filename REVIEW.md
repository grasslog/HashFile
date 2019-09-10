# CODE REVIEW

## magic number
避免 magic number 使用宏定义让代码更加清晰易读， eg: 
#define HASH_FILE_PATH "/home/grasslog/tmp/IP/ip_data.dat"
#define HASH_GENERATION_PATH "/home/grasslog/tmp/hashfile"
#define BUFFER_SIZE 20

const int num = 100000000;		// url number  
const int TOP_URL_NUMBER = 100;
const int HASH_FILE_NUMBER = 1000;
## illegal global variable name
全局变量的不合理使用让代码变的不“安全”，应该使用函数参数代替全局变量
## corner cases in URL
对于 URL 的各种 corner case 需要给出对应的处理
1. url 不合法
2. url 极端异常导致统计的异常，如: url 全相同，url 全不相同
## hash function parameter ascii hash
对于 hash key 的选择，把 ip 转化为 int 型数饶了弯路，直接使用 ascii hash 计算对应 hash key 出现的次数
## sort algorithm
排序算法使用 binary-tree 却无法处理退化，top url 的排序选择中使用了手写哦选择排序算法。
改进:  以小根堆维护 top-url ，使用堆排序算法对每个 hash-file 进行插堆操作