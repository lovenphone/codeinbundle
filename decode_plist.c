#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DICT_START		"<dict>"
#define DICT_END		"</dict>"
#define KEY_START		"<key>"
#define KEY_END			"</key>"
#define STRING_START	"<string>"
#define STRING_END		"</string>"
#define ARRAY_START		"<array>"
#define ARRAY_END		"</array>"
#define ANSWER			"<key>answer</key>"
#define CATEGORY		"<key>category</key>"
#define IMAGES			"<key>images</key>"

#define FILENAME		"test.plist"
#define DSTFILE			"WordData.lua"
#define MAXSIZE			128


// 去除字符串前后的空白
static void trim(char *str)
{
	int s = 0;
	int t = 0;
	int idx = 0;
	char *ptr = str;
	char *etr = NULL;

	// 找到str里面第一个非空字符的位置并记录
	while (*ptr != 0)
	{
		if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')
		{
			idx++;
			ptr++;
		}
		else
		{
			break;
		}
	}

	s = idx;
	etr = str + strlen(str) - 1;
	idx = strlen(str) - 1;

	// 找到str里面最后一个非空字符的位置并记录
	while (*etr != '\0')
	{
		if (*etr == ' ' || *etr == '\t' || *etr == '\n')
		{
			idx--;
			etr--;
		}
		else
		{
			break;
		}
	}

	t = idx;

	// 截取原字符里面去除前后空白的剩余部分
	memcpy(str, ptr, t - s + 1);
	str[t -s + 1] = '\0';
}


int main()
{
	FILE *fp = NULL;
	FILE *dfp = NULL;
	char *buf = NULL;
	char tmp[48] = {0};
	int idx = 1;

	fp = fopen(FILENAME, "r");
	if (NULL == fp)
	{
		printf("open file error!\n");
		return -1;
	}
	dfp = fopen(DSTFILE, "w");
	if (NULL == dfp)
	{
		printf("create file error!\n");
		return -1;
	}

	buf = (char *)malloc(sizeof(char) * MAXSIZE);
	if (NULL == buf)
	{
		printf("Malloc memory error!\n");
		return -1;
	}

	// 先写入wordData.lua文件的开头部分
	fputs("local wordData = {\n", dfp);
	//fputs("\t{wordID = 1, image = {}}\n", dfp);
    
    //trim(arr);
    //printf("%s, %d\n", arr, strlen(arr));

	// 开始解析plist文件
#if 1
	while (fgets(buf, MAXSIZE, fp) != NULL)
	{
		// 先去除前后的空白
        trim(buf);
		// printf("%s\n", buf);

        // 一个新的条目
		if (!strcmp(buf, DICT_START))
		{
			sprintf(tmp, "\t{wordID = %d, ", idx++);
			fputs(tmp, dfp);
		}

		if (!strcmp(buf, ANSWER))	// answer字段
		{
			fgets(buf, MAXSIZE, fp);
			trim(buf);
			buf += strlen(STRING_START);
			buf[strchr(buf, '<') - buf] = '\0';
			sprintf(tmp, "answer = \"%s\", ", buf);
			fputs(tmp, dfp);
		}
		else if (!strcmp(buf, CATEGORY))	// category字段
		{
			fgets(buf, MAXSIZE, fp);
			trim(buf);
			buf += strlen(STRING_START);
			buf[strchr(buf, '<') - buf] = '\0';
			sprintf(tmp, "category = \"%s\", ", buf);
			fputs(tmp, dfp);
		}
		else if (!strcmp(buf, IMAGES))	// images字段，image可能有多个
		{
			
			sprintf(tmp, "images = {");
			fputs(tmp, dfp);

			// 略过<array>
			fgets(buf, MAXSIZE, fp);
			// 先写入第一张图片，图片至少有两张
			fgets(buf, MAXSIZE, fp);
			trim(buf);
			buf += strlen(STRING_START);
			buf[strchr(buf, '<') - buf] = '\0';
			sprintf(tmp, "\"%s\"", buf);
			fputs(tmp, dfp);

			// 处理后面的图片
			while (1)
			{
				fgets(buf, MAXSIZE, fp);
				trim(buf);
				if (!strcmp(buf, ARRAY_END))
				{
					// 该条目的最后一张图片
					sprintf(tmp, "}},\n");
					fputs(tmp, dfp);
					break;
				}
				else
				{
					buf += strlen(STRING_START);
					buf[strchr(buf, '<') - buf] = '\0';
					sprintf(tmp, " ,\"%s\"", buf);
					fputs(tmp, dfp);
				}
			}
		}
		else
		{
		}
	}

	sprintf(tmp, "}");
	fputs(tmp, dfp);

	fclose(fp);
	fclose(dfp);
#endif

	return 0;
}
