docker build -t kotb/threadpool .
docker run --name threadpool -it -a STDIN -a STDOUT --rm kotb/threadpool
