#include <story.h>
#include <file.h>

int main()
{
File in("/dev/stdin");
while(in.resolve() != frOk || in.is_mounted() == false);
char str[11] = {0};
in.read((void*)str, 10);
printf("app: %s\n", str);
}