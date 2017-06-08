#include<netinet/in.h>    
#include<stdio.h>   
#include<string.h> 
#include<stdlib.h>    
#include<sys/socket.h>    
#include<sys/stat.h>    
#include<sys/types.h>    
#include<unistd.h>   
#include<fcntl.h> 

void send_new(int fd, char *msg) {
 int len = strlen(msg);
 if (send(fd, msg, len, 0) == -1) {
  printf("Error in send\n");
 }
}

void php_cgi(char* script_path, int fd) {
 //char * hdr = "HTTP/1.1 200 OK\n Server: Web Server in C\n Connection: keep-alive\n";
 //int lngth = strlen(hdr);
 //printf("%s\n", "aWA 3");
 send_new(fd, "HTTP/1.1 200 OK\n Server: Web Server in C\n Connection: keep-alive\n");
 //send_new(fd, "HTTP/1.1 200 OK\n Server: Web Server in C\n Connection: keep-alive\n", lngth, 0);
 dup2(fd, STDOUT_FILENO);
 printf("%s\n", "aWA 444");
 char script[500];
 strcpy(script, "SCRIPT_FILENAME=");
 strcat(script, script_path);
 putenv("GATEWAY_INTERFACE=CGI/1.1");
 putenv(script);
 putenv("QUERY_STRING=");
 putenv("REQUEST_METHOD=GET");
 putenv("REDIRECT_STATUS=true");
 putenv("SERVER_PROTOCOL=HTTP/1.1");
 printf("%s\n", "aWA 4");
 putenv("REMOTE_HOST=127.0.0.1");
 execl("/usr/bin/php-cgi", "php-cgi", NULL);
 printf("%s\n", "aWA 5");
}
    
int main() {    
   int create_socket, new_socket, fd, bytes_read, msglength, nf;    
   socklen_t addrlen;    
   int bufsize = 1024;    
   char *buffer = malloc(bufsize);    
   struct sockaddr_in address; 
   char *reqline[3], path[99999], data_to_send[bufsize];  
 
   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){    
      printf("The socket was created\n");
   }
    
   address.sin_family = AF_INET;    
   address.sin_addr.s_addr = INADDR_ANY;    
   address.sin_port = htons(8089);    
    
   if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){    
      printf("Binding Socket\n");
   }
   while (1) {  
   //printf("%s\n", "aWA 2");  

      if (listen(create_socket, 10) < 0) {    
         perror("server: listen");    
         exit(1);    
      }    
    
      if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {    
         perror("server: accept");    
         exit(1);    
      }   
      if (new_socket > 0){    
         printf("The Client is connected...\n");
      } 
    
      int pid = fork();
      if (pid < 0)
       error("ERROR on fork");
      if (pid == 0) {
        close(create_socket);
       // connection(newsockfd);
       // exit(0);
        int rcvd = recv(new_socket, buffer, bufsize, 0);   

        if (rcvd<0)    // receive error
          fprintf(stderr,("recv() error\n"));
        else if (rcvd==0)    // receive socket closed
          fprintf(stderr,"Client disconnected upexpectedly.\n");
        else    // message received
        { 
           printf("%s\n", buffer);
           reqline[0] = strtok (buffer, " \t\n");
           if ( strncmp(reqline[0], "GET\0", 4)==0 )
           {
              reqline[1] = strtok (NULL, " \t");
              reqline[2] = strtok (NULL, " \t\n");
              if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
              {
                  write(new_socket, "HTTP/1.0 400 Bad Request\n", 25);
              }
              else
              {
                  if ( strncmp(reqline[1], "/\0", 2)==0 )
                      reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

                  strcpy(path, ".");
                  strcpy(&path[strlen(".")], reqline[1]);
                  printf("file: %s\n", path);

                  if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
                  {
                      //char* arr = strtok (reqline[1], ".");
                     // printf("%s\n", arr);
                      //printf("reqline: %s\n", reqline[1]);
                      if(strstr(reqline[1], ".php")){
                        //printf("444444444444444444444\n");
                        php_cgi(path, new_socket);
                        //sleep(1);
                        //close(new_socket);
                        //exit(1);`
                      }
                      else{
                        int size = get_file_size(fd);
                        //printf("reqline :%s\n", reqline[1]);
                        if(strstr(reqline[1], ".pdf")){
                          //printf("%s\n", "PDF File ekak awaaaaa");
                          send(new_socket, "HTTP/1.0 200 OK\r\n", 17, 0);
                          send_new(new_socket, "Content-Type: application/pdf; charset=utf-8\r\n\r\n\0");
                          sendfile(new_socket, fd, NULL, size);
                        }
                        if(strstr(reqline[1], ".html")){
                          send(new_socket, "HTTP/1.0 200 OK\r\n", 17, 0);
                          send_new(new_socket, "Content-Type: text/html; charset=utf-8\r\n\r\n\0");
                          sendfile(new_socket, fd, NULL, size);
                        }
                        if(strstr(reqline[1], ".jpg")){
                          send(new_socket, "HTTP/1.0 200 OK\r\n", 17, 0);
                          send_new(new_socket, "Content-Type: image/jpg; charset=utf-8\r\n\r\n\0");
                          sendfile(new_socket, fd, NULL, size);
                        }
                        if(strstr(reqline[1], ".jpeg")){
                          send(new_socket, "HTTP/1.0 200 OK\r\n", 17, 0);
                          send_new(new_socket, "Content-Type: image/jpeg; charset=utf-8\r\n\r\n\0");
                          sendfile(new_socket, fd, NULL, size);
                        }
                        if(strstr(reqline[1], ".png")){
                          send(new_socket, "HTTP/1.0 200 OK\r\n", 17, 0);
                          send_new(new_socket, "Content-Type: image/png; charset=utf-8\r\n\r\n\0");
                          sendfile(new_socket, fd, NULL, size);
                        }
                        if(strstr(reqline[1], ".ico")){
                          send(new_socket, "HTTP/1.0 200 OK\r\n", 17, 0);
                          send_new(new_socket, "Content-Type: image/ico; charset=utf-8\r\n\r\n\0");
                          sendfile(new_socket, fd, NULL, size);
                        }
                        //while ( (bytes_read=read(fd, data_to_send, bufsize))>0 )
                         //   write (new_socket, data_to_send, bytes_read);
                      }
                  }
                  else    
                  {
                    nf=open("./not_found.html", O_RDONLY);
                    int size = get_file_size(nf);
                    //printf("%s\n", fd);
                    printf("404 File not found Error\n");
                    send(new_socket, "HTTP/1.1 404 Not Found\n\n", 24, 0);
                    sendfile(new_socket, nf, NULL, size);
                    //while ( (bytes_read=read(fd, data_to_send, bufsize))>0 )
                     //     write (new_socket, data_to_send, bytes_read);
                    //send(new_socket, "Server : Web Server in C\r\n\r\n", 29, 0);
                    //send_new(new_socket, "<html><head><title>404 Not Found</head></title>");
                    //send(new_socket, "<body><p>404 Not Found: The requested resource could not be found!</p></body></html>");
                    //write(new_socket, "<html><head><title>404 Not Found</head></title>", msglength); //FILE NOT FOUND
                  }
              }
           }
        }
        exit(0);
      } else
       close(new_socket);
      /* end of while */  
      //printf("%s\n", "aWA 1");
   }
   close(create_socket);    
   return 0;    
}

int get_file_size(int fd) {
 struct stat stat_struct;
 if (fstat(fd, &stat_struct) == -1)
  return (1);
 return (int) stat_struct.st_size;
}