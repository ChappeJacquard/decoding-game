#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_STRING_SIZE 10000
#define PTERROR(...) do{fprintf(stderr, "%s, %s (%d): ", __FILE__, __func__, __LINE__); fprintf(stderr, __VA_ARGS__);}while(0)
#define CRYPT(M, K)   (((M+K-64)%95)<0?95+(M+K-64)%95+32:(M+K-64)%95+32)
#define PRINTHELP() printf("This software crypt a message in a cipher with a key, all of them being in separate ASCII files of 10ko max. It can also decrypt a cipher into a message with a key. \
By default, those files are named message_file, cipher_file and key_file.\n \
List of options:\n \
\t-'m' to specify a different message file name\n \
\t-'k' to specify a different key file name\n \
\t-'c' to specify a different cipher name\n \
\t-'d' to decrypt a cipher into a message\n \
\t-'h' to print this help\n")

typedef struct{
	char* name;
	char txt[MAX_STRING_SIZE];
}text_t;

typedef enum{
	CRYPT = 0,
	DECRYPT = 1
}op_t;

typedef struct{
	text_t* msg;
	text_t* cipher;
	text_t* key;
	op_t* op;
	int size;
}data_t;

static int crypt_string(data_t* p_data);
static int decrypt_string(data_t* p_data);
static int read_file(text_t* file);
static int wubalubadubdub(data_t* p_data);

int main(int argc, char* argv[])
{
	int ret = 0;
	op_t operation = CRYPT;

	text_t msg;
	text_t cipher;
	text_t key;

	/* use default names for files if they are not passed as options */
	msg.name    = "message_file";
	key.name    = "key_file";
	cipher.name = "cipher_file";

	if(argc > 1)
	{
		for(int i = 1; (i < argc) && (argv[i][0] == '-'); i++)
		{
        	switch (argv[i][1])
			{
        		case 'm':
					if((argc > i+1))
					{
						msg.name = argv[i+1];
					}
					else
					{
						PTERROR("missing message file name \n");
						ret = -1;
					}
					break;
        		case 'k':
					if((argc > i+1))
					{
						key.name = argv[i+1];
					}
					else
					{
						PTERROR("missing key file name \n");
						ret = -1;
					}
					break;
        		case 'c':
					if((argc > i+1))
					{
						cipher.name = argv[i+1];
					}
					else
					{
						PTERROR("missing cipher file name \n");
						ret = -1;
					}
					break;
        		case 'd':
					operation = DECRYPT;
					break;
				case 'h':
					ret = -1;
        		default:
					ret = -1;
			}
		}
	}
	if(ret == -1)
	{
        PRINTHELP();
	}
	else
	{
		data_t data = {
		.msg    = &msg,
		.cipher = &cipher,
		.key    = &key,
		.size   = 0,
		.op		= &operation
		};

		ret = wubalubadubdub(&data);
	}
	return ret;
}

static int read_file(text_t* file)
{
	int ret=0;
	struct stat st;

	if(stat(file->name, &st))
	{
		ret = -1;
		PTERROR("stat() exited with error: %s\n", strerror(errno));
		printf("filename: %s\n", file->name);
	}
	else
	{
		if( st.st_size > MAX_STRING_SIZE)
		{
			ret = -1;
			PTERROR("%s is too big, max supported size is %d bytes\n", file->name, MAX_STRING_SIZE);
		}
		else
		{
			FILE* fp = fopen(file->name, "r");
			if(!fp)
			{
				ret = -1;
				PTERROR("fopen() exited with error: %s\n", strerror(errno));
			}
			else if(fread(file->txt, 1, st.st_size, fp) < st.st_size)
			{
				PTERROR("fread() exited with error: %s\n", strerror(errno));
				ret = -1;
			}
			else
			{
				/* do nothing */
			}
			fclose(fp);
		}
	}
	return ret;
}

static int crypt_string(data_t* p_data)
{
	int ret = 0; /* return variable */
	size_t l_key = 0; /* length of the key*/
	size_t l_msg = 0; /* length of the message*/

	if(p_data->msg->txt == NULL || p_data->key->txt == NULL || p_data->cipher->txt == NULL)
	{
		ret = -1;
		PTERROR("unexpected NULL pointer\n");
	}
	else
	{
		l_key        = strlen(p_data->key->txt);
		l_msg        = strlen(p_data->msg->txt);
		p_data->size = l_msg;

		for(int i=0;i < l_msg;i++)
		{
			p_data->cipher->txt[i] = (p_data->msg->txt[i]>31||p_data->msg->txt[i]<127)?CRYPT(p_data->msg->txt[i],p_data->key->txt[i%l_key]):p_data->msg->txt[i];
		}
	}
	return ret;
}

static int decrypt_string(data_t* p_data)
{
	int ret = 0; /* return variable */

	// exercice laissé au lecteur
	return ret;
}

static int wubalubadubdub(data_t* p_data)
{
	int ret = 0;
	if(p_data == NULL)
	{
		ret = -1;
		PTERROR("Unexpected NULL pointer \n");
	}
	else
	{
		int (*f)(data_t* p_data);
		text_t* p_toread;
		text_t* p_towrite;

		(*p_data->op == CRYPT)? \
		(f=crypt_string, p_toread=p_data->msg, p_towrite=p_data->cipher): \
		(f=decrypt_string, p_toread=p_data->cipher, p_towrite=p_data->msg);

		if(read_file(p_data->key) || read_file(p_toread))
		{
			ret = -1;
			PTERROR("read_file() exited with error\n");
		}
		else if(f(p_data))
		{
			ret = -1;
		}
		else
		{
			FILE* fp = fopen(p_towrite->name, "w");
			if(!fp)
			{
				PTERROR("fopen() exited with error: %s\n", strerror(errno));
				ret = -1;
			}
			else if(fwrite(p_towrite->txt, 1, p_data->size, fp) < p_data->size)
			{
				PTERROR("fwrite() exited with error: %s\n", strerror(errno));
				ret = -1;
			}
			else
			{
				/* do nothing */
			}
			fclose(fp);
		}
	}
	return ret;
}