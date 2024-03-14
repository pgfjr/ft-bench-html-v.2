#include "write_html.h"

static FILE* output_file = NULL;
static int table_count = 0;
static int column_count = 2;

void begin_table(int num_columns)
{
    ++table_count;

    if (table_count > 1)
    {
        fputs("<br>\n", output_file);
    }
    fputs("<table>\n", output_file);

    column_count = num_columns;
}

void end_table()
{
    fputs("</table>\n", output_file);
    column_count = 2;
}

void begin_row()
{
    fputs("<tr>\n", output_file);
}

void end_row()
{
    fputs("</tr>\n", output_file);
}

static void
write_html_ender()
{
    fputs("</body>\n</html>", output_file);
}

void close_html_file()
{
    if (output_file)
    {
        write_html_ender();
        fclose(output_file);
        output_file = stdout;
    }
}

static void
write_html_header(const char* fontfile)
{
    fprintf(output_file,
        "<html>\n"
        "<head>\n"
        "<title>\n"
        "ftbench %s\n"
        "</title>\n"
        "<style>\n"
        ".bg {"
        "background-color: rgb(150, 212, 212);"
        "}"
        "\ttable {\n"
        "\tborder-collapse: separate;\n"
        "\tborder-spacing: 10px 0;\n"
        "\t}\n"
        "\tth, td {\n"
        "\tpadding: 10px;\n"
        "border-bottom: 1px solid #DDD;"
        "}\n"
     //   "\ttr {\n"
       // "\tborder-bottom: 1px solid #ddd;\n"
       // "}\n"
        "</style>\n"
        "</head>\n"
        "<body>\n",
        fontfile);
}


FILE* create_html_file(const char* filename)
{
    FILE* tmp = fopen(filename, "w");

    if (tmp)
    {
        output_file = tmp;
        write_html_header(filename);
        return output_file;
    }
    return NULL;
}

char *trim_spaces(char* s)
{
    int len = (int)strlen(s);

    for (int i = len - 1; i >= 0; --i)
    {
        if (isspace(s[i]))
            s[i] = '\0';
        else
            break;
    }
    while (*s)
    {
        if (isspace(*s))
            ++s;
        else
            break;
    }
    return s;
}

char *next_cell(char* s)
{
    while (*s)
    {
        if (isspace(*s))
            ++s;
        else
        {
            break;
        }
    }
    if (*s == '\0')
        return NULL;
    return s;
}

void write_cell(char* s)
{
    char* colon = (char *)strchr(s, ':');

    fputs("<tr>\n", output_file);

    if (colon)
    {
        const char* cell2 = next_cell(colon + 1);

        if (cell2)
        {
            fprintf(output_file, "<td class='bg'>%.*s</td><td>%s</td>\n", (int)(colon - s), s, cell2);
        }
        else
        {
            fprintf(output_file, "<td class='bg' colspan='2'>%s</td>\n", s);
        }
    }
    else
    {
        fprintf(output_file, "<td class='bg' colspan='2'>%s</td>\n", s);
    }
    fputs("</tr>\n", output_file);
}

void write_table_cells(char *s)
{
    char* p = strtok(s, "\n");

    begin_table(2);
    while (p != NULL)
    {
        while (isspace(*p))
        {
            ++p;
        }
        write_cell(p);
        p = strtok(NULL, "\n");
    }
    end_table();
}

void write_table_body_with_head(char* s)
{
    char* p = strtok(s, "\n");

    if (p)
    {
        fprintf(output_file, "<tr><th colspan='%d' style='text-align: left;'>%s</th></tr>\n", column_count, p);

        while ((p = strtok(NULL, "\n")) != NULL)
        {
            while (isspace(*p))
            {
                ++p;
            }
            write_cell(p);
        }
    }
}

void split_cell(const char* s)
{
    const char* ptr = strstr(s, "us/op");

    if (ptr)
    {
        ptr += 5;

        fprintf(output_file, "<td style='text-align: right;'>%.*s</td>\n", (int)(ptr-s), s);
        fprintf(output_file, "<td style='text-align: right;'>%s</td>\n", ptr+1);
    }
    else
    {
        fprintf(output_file, "<td colspan='%d' style='text-align: right;'>%s</td>\n", column_count - 1, s);
    }
}

int
hprintf(int element,
    const char* format,
    ...)
{
    char buffer[1024], *ptr;
    va_list args;
    int count;

    va_start(args, format);
    count = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    //fputs(buffer, log_file);
    fputs(buffer, stdout);

    ptr = trim_spaces(buffer);

    switch (element)
    {
    case TABLE_TITLE:
        fprintf(output_file, "<h1>\n%s\n</h1>\n", ptr);
        break;
    case TABLE_CELLS:
        write_table_cells(buffer);
        break;
    case TABLE_BODY_WITH_HEAD:
        write_table_body_with_head(buffer);
        break;
    case TABLE_CELL_LEFT:
        fprintf(output_file, "<td class='bg' style='text-align: left;'>%s</td>\n", ptr);
        break;
    case TABLE_CELL_RIGHT_SPAN:
        fprintf(output_file, "<td colspan='%d' style='text-align: right;'>%s</td>\n", column_count - 1, ptr);
        break;
    case TABLE_CELL_RIGHT_SPLIT:
        split_cell(ptr);
        break;
    case TABLE_ROW:
        fputs("<tr>\n", output_file);
        write_cell(buffer);
        fputs("</tr>", output_file);
        break;
    case TABLE_HEADING:
        fprintf(output_file, "<tr><th colspan='%d' style='text-align: left;'>%s</th></tr>\n", column_count, ptr);
        break;
    }
    return count;
}

