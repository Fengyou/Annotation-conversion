#include <stdio.h>
#include <errno.h>
#include <assert.h>

typedef enum STATE
{
	SUCCESS,	// 成功
	FILE_ERROR,	// 文件错误
	NO_MATCH,	// 不匹配
	OTHER,		// 其他错误
}STATE;

typedef enum TAG
{
	TAG_BEGIN,		// 在C注释段中
	TAG_END,		// C注释结束
}TAG;

#pragma warning(disable:4996)

STATE AnnotationConvert(FILE* inFile, FILE* outFile)
{
	TAG tag = TAG_END;
	char firstCh, secondCh;
	assert(inFile);
	assert(outFile);

	do{
		firstCh = fgetc(inFile);
		switch (firstCh){
		case '/':
			secondCh = fgetc(inFile);
			if (secondCh == '*'&& tag == TAG_END)
			{
				fputc('/', outFile);
				fputc('/', outFile);

				tag = TAG_BEGIN;
			}
		
			else
			{
				fputc(firstCh, outFile);
				fputc(secondCh, outFile);
				if (secondCh == '/')
				{
					char next = fgetc(inFile);
					while ((next != '\n')&&(next!=EOF))
					{
						next = fgetc(inFile);
						fputc(next, outFile);
					} 
				}
			}
			break;
		case '\n':
			fputc('\n', outFile);
			if (tag == TAG_BEGIN)
			{
				fputc('/', outFile);
				fputc('/', outFile);
			}
			break;
		case '*':
			secondCh = fgetc(inFile);
			if (secondCh == '/'&&tag==TAG_BEGIN)
			{
				char next = fgetc(inFile);
				if (next != '\n' && next != EOF)
				{
					fseek(inFile, -1, SEEK_CUR);
				}
				if (next == EOF)
				{
					firstCh = EOF;
				}

				tag = TAG_END;
			}
			else
			{
				fputc(firstCh, outFile);
				fseek(inFile, -1, SEEK_CUR);
			}
			fputc('\n', outFile);
			break;
		default:
			fputc(firstCh, outFile);
			break;
		}
	} while (firstCh != EOF);

	if (tag == TAG_END)
	{
		return SUCCESS;
	}
	else
	{
		return NO_MATCH;
	}
}

int StartConvert()
{
	STATE s;
	const char* inFileName = "input.c";
	const char* outFileName = "output.c";

	FILE* inFile = fopen(inFileName, "r");
	FILE* outFile = fopen(outFileName, "w");

	if (inFile == NULL)
	{
		return FILE_ERROR;
	}

	if (outFile == NULL)
	{
		fclose(inFile);
		return FILE_ERROR;
	}

	s = AnnotationConvert(inFile, outFile);

	fclose(inFile);
	fclose(outFile);

	return s;
}

int main()
{
	STATE ret = StartConvert();

	if (ret == SUCCESS)
	{
		printf("转换成功\n");
	}
	else if (ret == NO_MATCH)
	{
		printf("不匹配\n");
	}
	else if (ret == FILE_ERROR)
	{
		printf("文件错误: %d\n", errno);
	}
	else
	{
		printf("其他错误: %d\n", errno);
	}

	return 0;
}
