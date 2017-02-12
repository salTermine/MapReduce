#include "lott.h"

static void* map(void*);
static void* reduce(void*);
F_stats *add_to_list(char *filename);

int part1()
{
    // VARIABLES
    errno = 0;
    pthread_t *threads = NULL;
    DIR *d_ptr;
    struct dirent *d_info;
    int count = 0;
    int i;

    // OPEN DIRECTORY
    if((d_ptr = opendir("data")) == NULL)
        unix_error("opendir error");

    if (errno != 0)
        unix_error("readdir error");

    // READ DIRECTORY CREATE ONE THREAD FOR EACH FILE
    while ((d_info = readdir(d_ptr)) != NULL) 
    {   
        if(d_info->d_type == DT_REG)
        {
            threads = realloc(threads, (sizeof(pthread_t) * (count + 1)));
            pthread_create(&(threads[count]), NULL, map, strdup(d_info->d_name));
            count++;
        }
    }

    // JOIN THREADS AND PRINT RESULTS
    for(i = 0; i < count; i++)
    {
        F_stats *fstats = NULL;
        pthread_join(threads[i], (void*)&fstats);

        // CALL REDUCE
        reduce((void*)&fstats);

        if(current_query == 0 || current_query == 1)
        {
            fprintf(stdout,"File: %s\nAvg. Duration: %lf\n\n", fstats->file_name, fstats->avg_dur);
        }
        else if(current_query == 2 || current_query == 3)
        {
            fprintf(stdout,"File: %s\nAvg. Users per year: %lf\n", fstats->file_name, fstats->avg_user);
        }
        else if(current_query == 4)
        {

        }
    }

    // PRINT OUT PART# AND QUERY
    printf(
        "Part: %s\n"
        "Query: %s\n",
        PART_STRINGS[current_part], QUERY_STRINGS[current_query]);

    // CLOSE DIRECTORY
    closedir(d_ptr);
    return 0;
}

//---------------------------- MAP --------------------------------
static void* map(void *v)
{
    return add_to_list((char*)v);
}

//---------------------------- REDUCE --------------------------------
static void* reduce(void* v)
{
    R_stats *ret = malloc(sizeof(R_stats));

    

    return ret;
}

//---------------------------- ADD TO LIST --------------------------------
F_stats *add_to_list(char *filename)
{
    F_stats *ret = malloc(sizeof(F_stats));

    char f_name[strlen("data/") + strlen(filename + 1)];
    strcpy(f_name, "data/");
    strcat(f_name, filename);
    
    size_t length = 0;
    char *line = NULL;
    char *broken_line = NULL;

    char *ts = NULL;
    char *ip = NULL;
    char *dur = NULL;
    char *c_code = NULL;

    int min_dur = 100;
    int max_dur = -1;
    int total = 0;
    int count = 0;

    int years[100] = {0};

    ret->file_name = strdup(filename);

    FILE *fp;
    fp = fopen(f_name, "r");

    if(fp != NULL)
    {
        while(getline(&line, &length, fp) != -1)
        {
            broken_line = strdup(line);
            ts = strsep(&broken_line, ",");
            ip = strsep(&broken_line, ",");
            dur = strsep(&broken_line, ",");
            c_code = strsep(&broken_line, ",");

            if(current_query == 0 || current_query == 1)
            {
                int d = atoi(dur);
                if(d < min_dur) min_dur = d;
                if(d > max_dur) max_dur = d;
                total += atoi(dur);
                count++;
            }
            else if(current_query == 2 || current_query == 3)
            {
                time_t date = 0;
                int yr_idx = 0;
                char *str_date = NULL;
                char *yr_str = NULL;

                date = atoi(ts);
                str_date = ctime(&date);
                yr_str = str_date + (strlen(str_date) - 3);

                yr_idx = atoi(yr_str);
                years[yr_idx]++;

            }
            else if(current_query == 4)
            {
                fprintf(stdout,"IP: %s\n CC: %s\n", ip, c_code);
            }
        }
        free(line);
        fclose(fp);
    }
    else
    {
        printf("File does not exist.\n");
        return 0;
    }

    if(current_query == 0 || current_query == 1)
    {
        ret->avg_dur = (double)total / count;
    }
    else if(current_query == 2 || current_query == 3)
    {
        int i;
        int year_count = 0;

        for(i = 0; i < 100; i++)
        {
            if(years[i] != 0)
            {
                total += years[i];
                year_count++;
            }
        }
        ret->avg_user = (double)total / year_count;
    }
    else if(current_query == 4)
    {

    }
    return ret;
}



//---------------------------- UNIX ERROR --------------------------------
void unix_error(const char *msg)
{
    int errnum = errno;
    fprintf(stderr, "%s (%d: %s)\n", msg, errnum, strerror(errnum));
    exit(EXIT_FAILURE);
}