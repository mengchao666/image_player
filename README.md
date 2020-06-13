## 一.用法：
一.解压lib下的两个压缩包
tar xzf libjpeg-turbo-1.2.1.tar.gz
tar xzf libpng-1.6.37.tar.gz



## 二.交叉编译libjpeg

cd libjpeg-turbo-1.2.1
./configure --prefix=/work/projects/libjpeg-turbo-1.2.1/tmp/ --host=arm-linux
make
make install 	



## 三.交叉编译libpng

cd libpng-1.6.37/
./configure --prefix=/work/projects/libpng-1.6.37/tmp/ --host=arm-linux
make
make install 	



## 四.拷贝头文件和库文件到交叉编译器目录下

4.1把编译出来的头文件应该放入：
/usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include

cd  /work/projects/libjpeg-turbo-1.2.1/tmp/include
cp * /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include
cd  /work/projects/libpng-1.6.37/tmp/include
cp * /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include

4.2把编译出来的库文件应该放入：
/usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/armv4t/lib

cd  /work/projects/libjpeg-turbo-1.2.1/tmp/lib
cp *so* -d /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/armv4t/lib
cd  /work/projects/libpng-1.6.37/tmp/lib
cp *so* -d /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/armv4t/lib



## 五.拷贝.so文件到开发板上

cd  /work/projects/libjpeg-turbo-1.2.1/tmp/lib
cp *.so* /work/nfs_root/fs_mini_mdev_new/lib/ -d
cd  /work/projects/libpng-1.6.37/tmp/lib
cp *.so* /work/nfs_root/fs_mini_mdev_new/lib/ -d



## 六.执行方法

到顶层目录下执行make,将生成的PicPrc拿到开发板上运行
运行方法：
        转化格式   图像文件		       缩放比例				旋转角度
`PicPrc <format>     <file>     <size 0-10 of zoom>     <angle 0-360 of rotate>`
例：./PicPrc png2rgb test.png 2.2 222.2