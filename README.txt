�÷���
һ.��ѹlib�µ�����ѹ����
tar xzf libjpeg-turbo-1.2.1.tar.gz
tar xzf libpng-1.6.37.tar.gz



��.�������libjpeg
cd libjpeg-turbo-1.2.1
./configure --prefix=/work/projects/libjpeg-turbo-1.2.1/tmp/ --host=arm-linux
make
make install 	



��.�������libpng
cd libpng-1.6.37/
./configure --prefix=/work/projects/libpng-1.6.37/tmp/ --host=arm-linux
make
make install 	



��.����ͷ�ļ��Ϳ��ļ������������Ŀ¼��
4.1�ѱ��������ͷ�ļ�Ӧ�÷��룺
/usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include

cd  /work/projects/libjpeg-turbo-1.2.1/tmp/include
cp * /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include
cd  /work/projects/libpng-1.6.37/tmp/include
cp * /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include

4.2�ѱ�������Ŀ��ļ�Ӧ�÷��룺
/usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/armv4t/lib

cd  /work/projects/libjpeg-turbo-1.2.1/tmp/lib
cp *so* -d /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/armv4t/lib
cd  /work/projects/libpng-1.6.37/tmp/lib
cp *so* -d /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/armv4t/lib



��.����.so�ļ�����������
cd  /work/projects/libjpeg-turbo-1.2.1/tmp/lib
cp *.so* /work/nfs_root/fs_mini_mdev_new/lib/ -d
cd  /work/projects/libpng-1.6.37/tmp/lib
cp *.so* /work/nfs_root/fs_mini_mdev_new/lib/ -d



��.
������Ŀ¼��ִ��make,�����ɵ�PicPrc�õ�������������
���з�����
        ת����ʽ   ͼ���ļ�		       ���ű���				��ת�Ƕ�
PicPrc <format>     <file>     <size 0-10 of zoom>     <angle 0-360 of rotate>
����./PicPrc png2rgb test.png 2.2 222.2