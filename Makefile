all: gitsh

gitsh: prompt.o shell.o commandhandler.o echo.o pwd.o cd.o ls.o systemcommand.o pinfo.o history.o nightswatch.o redirection.o environment.o pipe.o jobs.o jobkill.o
	gcc -Wall -o gitsh prompt.o shell.o commandhandler.o echo.o pwd.o cd.o ls.o systemcommand.o pinfo.o history.o nightswatch.o redirection.o environment.o pipe.o jobs.o jobkill.o

clean:
	rm gitsh *.o