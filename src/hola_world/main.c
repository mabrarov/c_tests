#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static char *str_alloc(size_t len)
{
	return (char *) malloc(len + 1);
}
 
static void str_free(char **s)
{
	if (NULL != s)
	{
		free(*s);
		*s = NULL;
	}
}

static char *str_cpy(char **dst, const char *src)
{
	char *temp = NULL;
	if (NULL != dst)
	{
		if (NULL != src)
		{
			const size_t src_len = strlen(src);
			if (NULL == (temp = str_alloc(src_len)))
			{
				return NULL;
			}
			memcpy(temp, src, src_len + 1);
		}
		str_free(dst);
		*dst = temp;
	}  
	return temp;
}

static size_t str_len(const char *s)
{
	return NULL == s ? 0 : strlen(s);
}

static char *str_cat(char **left, const char *right) 
{
	char *temp = NULL;
	if (NULL != left)
	{
		if (NULL != *left || NULL != right)
		{            
			const size_t left_len  = str_len(*left);
			const size_t right_len = str_len(right);
			if (NULL != (temp = str_alloc(left_len + right_len)))
			{
				memcpy(temp, *left, left_len);
				memcpy(temp + left_len, right, right_len + 1);
				str_free(left);
				*left = temp;
			}
		}
	}
	return temp;
}

static const char *placeholder = "%s";
static const size_t placeholder_len = 2;

static size_t count_placeholders(const char *fmt)
{
	size_t count = 0;
	if (NULL != fmt)
	{
		while (*fmt)
		{
			fmt = strstr(fmt, placeholder);
			if (NULL == fmt)
			{
				break;
			}
			++count;
			fmt += placeholder_len;    
		}
	}
	return count;
}

struct fmt_part_t
{
	const char *s;
	size_t len;
};

char *fmt_part_cat(size_t total_len, const struct fmt_part_t *parts, 
		size_t count)
{
	char *const dst = str_alloc(total_len);
	if (NULL != dst)
	{
		char *str = dst; 
		while (count)
		{
			memcpy(str, parts->s, parts->len);
			str += parts->len;
			++parts;
			--count;
		}
		*str = '\0';
	}
	return dst;
}

size_t str_printf(char **dst, const char *fmt, ...)
{
	size_t dst_len = 0;
	if (NULL != dst)
	{
		char *temp = NULL;
		if (NULL != fmt)
		{
			const size_t part_count = count_placeholders(fmt) * 2 + 1;
			struct fmt_part_t *const parts = (struct fmt_part_t *) malloc(
					sizeof(struct fmt_part_t) * part_count);
			if (NULL == parts)
			{
				return 0;
			}
			else
			{
				size_t  used_part_count = 0;
				size_t  fmt_len = strlen(fmt);
				va_list arg_list;      
				va_start(arg_list, fmt);
				while (fmt_len)
				{
					const char *placeholder_pos = 
						placeholder_len > fmt_len ? NULL : strstr(fmt, placeholder);
					const size_t fmt_part_len = 
						placeholder_pos ? placeholder_pos - fmt : fmt_len;
					if (fmt_part_len)
					{            
						parts[used_part_count].s   = fmt;
						parts[used_part_count].len = fmt_part_len;
						++used_part_count;
						dst_len += fmt_part_len;
						fmt     += fmt_part_len;
						fmt_len -= fmt_part_len;            
					}
					if (NULL != placeholder_pos)
					{
						const char *arg = va_arg(arg_list, const char *);
						if (NULL != arg)
						{
							const size_t arg_len = strlen(arg);
							parts[used_part_count].s   = arg;
							parts[used_part_count].len = arg_len;
							++used_part_count;
							dst_len += arg_len;              
						}
						fmt      = placeholder_pos + placeholder_len;
						fmt_len -= placeholder_len;            
					}
				}
				va_end(arg_list);
				temp = fmt_part_cat(dst_len, parts, used_part_count);
				free(parts);
				if (NULL == temp)
				{
					return 0;
				}
			}
		}
		str_free(dst);
		*dst = temp;
	}
	return dst_len;
}

int main(int argc, char *argv[])
{
	char *s = NULL;    
	str_cpy(&s, "Hola Hola");
	str_cpy(&s, s + 5);
	str_cat(&s, " World");
	str_printf(&s, "%s!", s);
	puts(s); /* result: "Hola World!" */
	str_free(&s);

	(void) argc;
	(void) argv;
	return EXIT_SUCCESS;
}
