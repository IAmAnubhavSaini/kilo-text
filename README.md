# kilo-text

```bash
docker build --tag f0c1s/kilo-text .
docker run --rm --interactive --tty f0c1s/kilo-text bash
# hint: press q to exit

# clean-up
docker ps | sed '1d' | grep "_" | grep "minutes ago" | awk '{print $1}' | xargs -n1 docker stop
docker ps -a | sed '1d' | grep "_" | grep "minutes ago" | awk '{print $1}' | xargs -n1 docker rm
```
