OBJDIRS=hal libc core
FILES=$(foreach dir, $(OBJDIRS), $(dir)/*.o)
VERSION=0.45

all:
	@echo "WARNING: If it does not compile, check path in template.mk!"
	@make -C hal
	@make -C core
	@make -C libc
	@make -C api
	@make -C apps
	@echo "Linking kernel"
	@ld -e start -Ttext 0x200000 $(FILES) -o output/kernel --oformat elf32-i386	
	
	@expr `cat .build` + 1 > .build
	@echo "/* include/version.h" > include/version.h
	@echo "   Generated automatically, do not modify!*/" >> include/version.h
	@echo "#define VERSION \"$(VERSION)\"" >> include/version.h
	@echo "#define BUILD `cat .build`" >> include/version.h
	
	@echo "Building tree for ISO image"
	@rm -rf disk/*
	@rm -f disk.iso
	@mkdir -p disk/boot/grub
	@cp grub/* disk/boot/grub
	@cp output/* disk/
	@cp COPYING disk/
	
	@echo "Building ISO image"
	@mkisofs -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -iso-level 3 -r -J -input-charset koi8-r -publisher "Peter Zotov <peterzotov@yandex.ru>" -o disk.iso disk
	
clean:
	@find ./ -name "*.o" -exec rm {} \;
	@find ./ -name "*~" -exec rm {} \;
	@rm -rf disk/*
	@rm -f disk.iso *.bz2
	@rm -rf output/*
	@echo "Cleaned"
	
binary_pkg:
	@make
	@echo "Packing binary"
	@bzip2 -9 disk.iso
	@mv disk.iso.bz2 story$(VERSION).iso.bz2
	
source_pkg:
	@echo "Packing source"
	@make clean
	@../pack.sh $(VERSION)
	@echo "Done"