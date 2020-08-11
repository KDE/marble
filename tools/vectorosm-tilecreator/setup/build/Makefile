build: image
	mkdir -p output
	docker run -it --rm -v `pwd`/output:/output vkrause/marble-builder build-tilegenerator.sh

image: Dockerfile build-tilegenerator.sh
	docker build -t vkrause/marble-builder .
