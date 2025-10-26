IMAGE_NAME = ophanim
CONTAINER_NAME = ophanim
PROC_PATH = /host_proc
NET_NI = wlo1

all: run

stop:
	@docker stop $(CONTAINER_NAME) 2>/dev/null || true
	@docker rm $(CONTAINER_NAME) 2>/dev/null || true

build:
	@docker build -t $(IMAGE_NAME):latest .

run: stop build
	@docker run -d --name $(CONTAINER_NAME) \
		--net=host --pid=host \
		-v /proc:$(PROC_PATH):ro \
		-e PROC_PATH=$(PROC_PATH) \
		-e NET_NI=$(NET_NI) \
		$(IMAGE_NAME):latest

logs:
	@docker logs -f $(CONTAINER_NAME)
