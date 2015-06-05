#include "network.h"

using namespace std;

typedef struct{
    pthread_t thread_tid;
    long thread_count;
}Thread;

Thread * tptr;

const int MAXNCLI = 50;
int clifd[MAXNCLI],iget,iput;
pthread_mutex_t clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clifd_cond = PTHREAD_COND_INITIALIZER;
static int nthreads;

void * thread_main(void * arg)
{
    int connfd,n;
    char buff[MAXLINE];
    printf("thread %d starting \n",(int)arg);
    for(;;)
    {
        pthread_mutex_lock(&clifd_mutex);
        while (iget == iput)
                    pthread_cond_wait(&clifd_cond, &clifd_mutex);
        connfd = clifd[iget];
        iget = (iget + 1)%MAXNCLI;
        pthread_mutex_unlock(&clifd_mutex);
        tptr[(int)arg].thread_count ++ ;
        while(true)
        {
            if((n = read(connfd,buff,MAXLINE)) == 0)
            {
                close(connfd);
                break;
            }
            else if(n == -1)
            {
                cout << "read error"
                     << endl;
                break;
            }
            else
            {
                cout << buff
                     << " in thread "
                     << (int)arg
                     << endl;
                write(connfd,buff,n);
            }
        }
    }
}

void thread_make(int i)
{
    pthread_create(&tptr[i].thread_tid,NULL,thread_main,(void *)i);
    return;
}

int main(int argc,char **argv)
{
    int i,listenfd,connfd;
    sockaddr_in servaddr;

    if(argc != 2 )
    {
        cout << "argc is wrong "
             << endl;
        exit(1);
    }

    nthreads = atoi(argv[1]);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
    listen(listenfd,LISTENQ);

    tptr = (Thread *)calloc(nthreads,sizeof(Thread));
    iget = iput = 0;

    for(int i = 0;i <nthreads;i ++)
        thread_make(i);

    for(;;)
    {
        connfd = accept(listenfd,NULL,NULL);
        pthread_mutex_lock(&clifd_mutex);
        clifd[iput] = connfd;
        iput = (iput +1)%MAXNCLI;
        if(iput == iget)
        {
            cout << "iput == iget error"
                 << endl;
            exit(1);

        }
        pthread_cond_signal(&clifd_cond);
        pthread_mutex_unlock(&clifd_mutex);
    }

    return 0;
}
