### 关于Qt添加mysql驱动

一些要点：

- 首先确保qt的环境属于正常状态，其中MinGW的版本要跟mysql的版本确认好，统一64位或32位。
- mysql的账号密码以及一些用户参数。
- qt中的相关资源模块环境都应该是完整的，比如src文件必须要有，src文件包括相关的一些插件原件，只不过需要自行编译如mysql驱动的添加。![image-20200923205125231](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923205125231.png)如果没有该文件需要重装qt。

首先在确保有相关的环境以及资源文件后开始添加驱动。

首先在qt的源文件中找到mysql的插件.pro文件：

![image-20200923205930957](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923205930957.png)

打开后作下面的一些变动：

1. 添加相关代码

   ```
   INCLUDEPATH += H:/qtmysql/ms8.0/include/
   LIBS += H:/qtmysql/ms8.0/lib/libmysql.lib
   ```

2. 注销代码（第6行）QMAKE_USE += mysql![image-20200923210153371](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923210153371.png)

3. 更改qsqldriverbase.pri中的include语句：

   ```
   include(./configure.pri)
   ```

   ![image-20200923210335364](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923210335364.png)

完成上述操作后编译，若出现下列错误表示mysql的驱动文件编译生成成功：![image-20200923210444573](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923210444573.png)

其生成的在跟qt同一根目录的文件下：

![image-20200923210659663](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923210659663.png)

随后将编译生成的两个文件qsqlmysql.dll和qsqlmysqld.dll放入mingwz的插件数据库驱动文件夹中，注意版本要对号。

![image-20200923210945724](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923210945724.png)

随后添加动态库：

![image-20200923211238492](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923211238492.png)

上述所有操作完成后可以测试连接，测试连接代码：

```c++
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>

QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("10.3.40.113");
    db.setPort(3306);
    db.setDatabaseName("test");
    db.setUserName("ioomie");
    db.setPassword("ioomie");
    bool ok = db.open();
    if (ok){
        qDebug()<<"open success";
        return true;
    }
    else {
        qDebug()<<"open fail";
        qDebug()<<"error open database because"<<db.lastError().text();
        return false;
    }
```

当用上述测试代码创建连接并成功后会在控制台输出：

![image-20200923211531915](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200923211531915.png)