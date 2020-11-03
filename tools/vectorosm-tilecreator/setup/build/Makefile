build: image
	mkdir -p output
	docker run -it --rm -v `pwd`/output:/output vkrause/marble-builder build-tilegenerator.sh
	docker run -it --rm -v `pwd`/output:/output vkrause/marble-builder build-tirex.sh

image: Dockerfile build-tilegenerator.sh build-tirex.sh
	docker build -t vkrause/marble-builder .
