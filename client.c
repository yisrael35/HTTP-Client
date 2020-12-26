#include <stdio.h>
#include <stdlib.h>
#include <string.h>        
#include <unistd.h>        	          // for read/write/close
#include <sys/types.h>     	          /* standard system types        */
#include <netinet/in.h>    	          /* Internet address structures */
#include <sys/socket.h>   	          /* socket interface functions  */
#include <netdb.h>   
#include <stdbool.h>      	          /* host to IP resolution            */

#define  BUFLEN 200                   /* maximum response size     */

/* Yisrael Bar 18-23/11/20 */


//in case of system call
void error(char* str){
    perror(str);
    exit(EXIT_FAILURE) ;
}
//check if the host name is null
void vaildHostName(char * host_name){
    if (host_name == NULL)
    {
        // printf("didn't found valid url\n");
        fprintf(stderr,"didn't found valid url\n");
        exit(EXIT_FAILURE);
    }
}

//in case of wrong command usage
void badUseg(){
    fprintf(stderr,"Usage: client [-p <text>] [-r n < pr1=value1pr2=value2 ...>] <URL>\n");
}



int main(int argc, char *argv[])
{
    int sockfd;
    char readbuf[BUFLEN];
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //in case there is not enough parameter that pass to the progrem
    if (argc < 2) {
        fprintf(stderr, "Missing server name \n");
        exit(EXIT_FAILURE) ; 
    }

    //SOCK_STREAM = tcp
    //create client socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("Socket failed\n");

    //---------------------------------------------scan argv start
    //scan argv to know what commend we been given
    // Strchr- char *strchr(const char *str, int c) ---search a char
    // Strstr-  char *strstr(const char *haystack, const char *needle)-- search a string
    // Strtok-  char *strtok(char *str, const char *delim) ---- cut a string
    int request_length = 0;
    bool flag_p = false;
    bool flag_r = false;
    char* host_name = NULL;
    int index_p = -1, index_r = -1, r_values = 0;
    int port = 80;
    //in case there is more then url (-p / -r)
    if(argc > 2){
        for (int i = 1; i < argc; i++)
        {   
            request_length += strlen(argv[i]);
            //check if there is -p
            if (strcmp(argv[i], "-p") == 0)
            {
                flag_p = true;
                index_p = i;
                //-p
                request_length -= 2;
                if (index_p + 1 == argc )
                {
                    printf("-p is missing a parametr\n");
                    badUseg();
                    exit(1);
                }
                
            }
            //check if there is an -r
            else if (strcmp(argv[i], "-r") == 0)
            {
                flag_r = true;
                index_r = i;
                if (index_r + 1 == argc )
                {
                    printf("-r is missing a parametr\n");
                    badUseg();
                    exit(1);
                }
                r_values = atoi(argv[i+1]);
                request_length -= 3;
                //check zero -atoi-------------------------------------
                if (r_values == 0)
                {
                    if (strcmp("0",argv[i+1]) == 0)
                    {
                        /* it's ok  */
                    }else
                    {
                        //check that it is not -p and after that -r
                        if (strcmp("-p", argv[i-1])==0)
                        {
                            flag_r = false;
                            continue;
                        }
                        
                        printf("After -r must be a postive number\n");
                        badUseg();
                        exit(1);
                    }
                }
                if (r_values < 0 )
                {
                    
                    printf("After -r must be a postive number\n");
                    badUseg();
                    exit(1);
                }
                if (r_values > argc - i -2){
                    printf("Not enough values\n");
                    badUseg();
                    exit(1);
                }
                if (i-1 == index_p)
                {
                    printf("r is value of p and r also have permetres\n");
                    badUseg();
                    exit(1);
                }
                
            }
            //search for host name
            char * temp = NULL;
            temp = strstr(argv[i], "http://");
            if (temp == NULL)
            {
                temp = strstr(argv[i], "HTTP://");
            }
            
            if (temp != NULL)
            {
                host_name = temp;
                //check if it's a value
                if (strstr(host_name, "=")!= NULL)
                {
                    host_name = NULL;
                }
                if ( i == index_p + 1 )
                {
                    host_name = NULL;
                }
                
            }

        }
        if (flag_r)
        {
            //need to check if there a value after =-------------------------
            //check if the r values have '='
            for (int i = 0; i < r_values ; i++)
            {
                if (argc > i+2+index_r  && strstr(argv[i+2+index_r], "=") == NULL)
                {
                    printf("There is a problame with r values\n");
                    badUseg();
                    exit(1);
                }
                if (argc > i+2+index_r  && strstr(argv[i+2+index_r], "=") != NULL)
                {
                    char * temp =  strstr(argv[i+2+index_r], "=");
                    if (strlen(temp)<=1)
                    {
                        printf("There is a problame with r values\n");
                        badUseg();
                        exit(1);       
                    }
                }   
            }
            //check the num of parameters
            if (argc > 2+index_r+ r_values && strstr(argv[2+index_r+ r_values], "=") != NULL)
            {
                printf("Too many parameters\n");
                badUseg();
                exit(1);
            }
            
        }
        
        
    }
    //in case we got only url
    else
    {
       //search for host name
        host_name = strstr(argv[1], "http://");
         if (host_name == NULL)
            {
                host_name = strstr(argv[1], "HTTP://");
            }
        vaildHostName(host_name);
        //  GET_length = 3;
        //  host_lenth = 5;
        //  HTTP/1.0 = 1;
        request_length += strlen(host_name) + 3 + 5 + 1;
    }
    //---------------------------------------------scan argv end
    
    //------------------------------------hendle the url/ path/ port start
    //hendle the url
    vaildHostName(host_name);
    // host_name = strstr(host_name, "www");

    //cut the http:// from host_name
    host_name = &host_name[7];
    // printf("host_name: %s\n\n",host_name);
    vaildHostName(host_name);

    //check if there is a path
    bool flag_path = false;
    char * path = NULL;
    path = strchr(host_name, '/');
    if (path != NULL)
    {   
        char * temp = (char*)malloc(strlen(path)*sizeof(char)+1);
        if(temp == NULL){
            printf("ERROR with path malloc\n");
            exit(1);
        }
        strcpy(temp, path);
        path = temp;

        flag_path = true;
    } 
    // printf ("path is: %s\n", path);

    //check if there is a port
    char* path_and_port = NULL;
    path_and_port = strchr(host_name , ':');
    //clean the port to be number: 1234
    if (path_and_port != NULL)
    {
        char* token;
        token = strtok(path_and_port, ":  /");
        token = strtok(path_and_port, ":  /");
        if (token == NULL)
        {
            if(flag_path)
                free(path);
            fprintf(stderr,"problame with the port number\n");//wrong input
            exit(EXIT_FAILURE);
        }
        //convert the port 
        port = atoi(token); 
        if (port <= 0)
        {
            if(flag_path)
                free(path);
            fprintf(stderr,"problame with the port number\n");//wrong input
            exit(EXIT_FAILURE);
        }
        
        // printf("the port is: %d\n",port);
    }
    //clean the host name to be: www.example.com
    host_name = strtok(host_name, ":  /");
    vaildHostName(host_name);

    // printf("host name: %s\n\n", host_name);        
    //------------------------------------hendle the url/ path/ port- end

    //------------------ connection to server-  start
    // connect to server 

    server = gethostbyname(host_name);
    if (server == NULL) {
        // error("ERROR, no such host\n");
        if(flag_path)
                free(path);
        herror("ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }
    int rc; /* system calls return value storage */
    #define h_addr h_addr_list[0]
    //init - struct sockaddr_in serv addr
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(port);

    ///problame with wrong port number - it stuck
    rc = connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));

    if (rc < 0) {
        if(flag_path)
            free(path);
        error("connect failed:\n");
    }
    //------------------ connection to server -end

   //update the request length
    //+5 host:, +1 HTTP/1.0 (http:// - HTTP/1.0), +3 GET/ +4 POST,+r_values (? &*r_values-1), +16 content-length:n 
    if (flag_r && flag_p)
    {
        //  content_length = 16;
        //  post_length = 4;
        //  host_lenth = 5;
        //  HTTP/1.0 = 1;
        request_length +=  16 + 4 + 5 +1 + r_values; 
    }
    else if (flag_p)
    {
        //  content_length = 16;
        //  post_length = 4;
        //  host_lenth = 5;
        //  HTTP/1.0 = 1;
        request_length +=  16 + 4 + 5 +1;
    }
    else if (flag_r)
    {
        // GET_length = 3;
        //  host_lenth = 5;
        //  HTTP/1.0 = 1;
        request_length += 3 + 5 + 1 + r_values;
    }
    //add to request length space and \r\n
    request_length +=15;
    
    //------------------------------------build request -start
    // build the request
    char* request = (char*)malloc(request_length*sizeof(char));
    if(request == NULL){
            // printf("ERROR with request malloc\n");
            error("ERROR with request malloc\n");
        }
    for (int i = 0; i < request_length; i++)
    {
        request[i] = '\0';
    }
    
    if (flag_p)
    {
        strcat(request , "POST ");  
    }else
    {
        strcat(request , "GET ");  
    }
    if (flag_path)
    {
        strcat(request , path);    
        free(path);      
    }
    if (flag_r)
    {
        strcat(request , "?");  
        for (int i = 0; i < r_values; i++)
        {
            strcat(request, argv[i+index_r+2]);
            if (i+1 != r_values)
            {
                strcat(request , "&");  
            } 
        }
    }
    strcat(request , " HTTP/1.0\r\nHost: ");
    strcat(request , host_name);  
    strcat(request , "\r\n");
    //in case there is -p we write it to the request
    if (flag_p)
    {
        strcat(request , "Content-length:");
        //convert the length of the word of -p to temp
        char temp[10];
        int temp_num = (int)strlen(argv[index_p+1]);
        sprintf(temp,"%d", temp_num);
        strcat(request , temp);  
        strcat(request , "\r\n\r\n");
        strcat(request , argv[index_p+1]);  
    }
    else strcat(request , "\r\n");
    // end of the requset build
    //------------------------------------------------


    rc = 0;
    //print the request to the screen
    printf("HTTP request =\n%s\nLEN = %d\n", request,(int) strlen(request));
    // send and then receive messages from the server
    rc = write(sockfd, request, request_length);
    if (rc == 0) 
        error("write() failed\n");
    free(request);
    //read from server the response--- start
    int size = 0;
    do
    {
	    rc = read(sockfd, readbuf,  BUFLEN);
        size += rc;
        if(rc > 0)
            {
	    	  readbuf[rc] = '\0';
              printf("%s\n", readbuf);
            }
	    if (size == 0) 
              error("read() failed");
    } while (rc > 0);
    //after the response - print the total bytes recevied
    printf("\nTotal received response bytes: %d\n",size);
    //read from server the response--- end

    close(sockfd);

    return EXIT_SUCCESS;
}

