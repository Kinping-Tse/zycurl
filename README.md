# ZYcURL

ZYcURL 是一个类似于官方PHP的curl扩展, 不过其实现了资源的常驻, 可保持socket长连接, 提升请求性能

## Requirements

* PHP 7.0+
* Linux, OS X, (Other unix system) ...
* curl PHP扩展的预安装

## Installation

    git clone https://github.com/Kinping-Tse/zycurl.git
    cd zycurl
    phpize
    ./configure
    make && make install

## API

目前只提供了以下基本的方法, 用法跟官方PHP扩展curl的方法是类似的, 不太相同的是setopt,getinfo的选项没有完全实现, 以及exec方法直接返回请求结果, 无需设置CURLOPT_RETURNTRANSFER。

- resource zycurl_init([string $url])
- bool zycurl_setopt(resource $res, int $opt_name, $opt_value)
- bool zycurl_setopt_array(resource $res, array $options)
- string zycurl_exec(resource $res)
- mixed zycurl_getinfo(resource $res [, int $option])
- void zycurl_close(resource $res)
- int zycurl_errno(resource $res)
- string zycurl_error(resource $res)
- string zycurl_strerror(int $err_code)
- void zycurl_reset(resource $res)
