build: image
	mkdir -p output
	docker run -it --rm -v `pwd`/output:/output vkrause/marble-builder build-tilegenerator.sh
	docker run -it --rm -v `pwd`/output:/output vkrause/marble-builder build-tirex.sh

image: Dockerfile build-tilegenerator.sh build-tirex.sh test-tilegenerator.sh
	docker build -t vkrause/marble-builder .

TIREX_CACHE_DIR=/k/osm/cache
test: image
	docker run -it --rm -v `pwd`/output:/output -v $(TIREX_CACHE_DIR):/var/lib/tirex/cache vkrause/marble-builder test-tilegenerator.sh
