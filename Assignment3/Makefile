CC ?= gcc209
CFLAGS += -g

STUDENT_ID := $(shell cat STUDENT_ID)
SUBMIT_DIR := .submit
SUBMIT_FILES:= customer_manager1.c customer_manager2.c readme EthicsOath.pdf
SUBMIT := $(STUDENT_ID)_assign3.tar.gz

TARGET := client1 clientmy1 client2 clientmy2

all: $(TARGET)

client1: client.c customer_manager1.c
	$(CC) $(CFLAGS) -o $@ $^

clientmy1: clientmy.c customer_manager1.c
	$(CC) $(CFLAGS) -o $@ $^

client2: client.c customer_manager2.c
	$(CC) $(CFLAGS) -o $@ $^

clientmy2: clientmy.c customer_manager2.c
	$(CC) $(CFLAGS) -o $@ $^

submit:
	mkdir -p $(SUBMIT_DIR)
	cp $(SUBMIT_FILES) $(SUBMIT_DIR)
	cd $(SUBMIT_DIR) && tar -czf ../$(SUBMIT) *
	rm -rf $(SUBMIT_DIR)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean submit
