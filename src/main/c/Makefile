all:
	gcc -std=c99 -O2 -Wall -DD_XOPEN_SOURCE=600 -DDEBUG -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/darwin -lm fjage-sentuator-i2c.c logging.c -o ../../../build/libs/libfjagesentuatori2c.jnilib -shared

clean:
	rm ../../../build/libs/libfjagesentuatori2c.*