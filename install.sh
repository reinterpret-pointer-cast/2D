#!/bin/bash

apt install -y clang \
				 make \
				 libwebp-dev \
				 llvm \
				 libfmt-dev \
				 libglfw3-dev \
				 libopus-dev \
				 libx11-dev

git clone https://github.com/7244/WITCH.git
mv WITCH /usr/local/include/WITCH

git clone https://github.com/7244/BCOL.git
mv BCOL /usr/local/include/BCOL

git clone https://github.com/7244/BLL.git
mv BLL /usr/local/include/BLL