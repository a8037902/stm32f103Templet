# stm32f103Templet

<h1>功能模块介绍</h1>

<p>模块
<br>业务代码在放在Module文件夹下，模块之间可以根据业务需要将业务分离在不同的线程中。通过消息传递数据。
<br>module.c文件根据需要填写模块的处理函数或注销。
</p>
<p>串口
<br>串口操作完全由中断完成，读写数据会缓存到一个循环缓冲区，无需占用线程时间。
</p>
<p>内存管理
<br>void * MEM_Get(u16 size, u8 * perr);
<br>u8 MEM_Put(void * pblk);
<br>内存分为大内存块和小内存块，数量和大小都可以在config.h里配置。
<br>在获取内存或者释放内存时，会根据你需要的内存大小和地址自动分配和释放。
</p>
<p>消息
<br>u8 MSG_Send(u8 task, u16 message, u16 para0, u16 para1, u8 * pData, u16 len);
<br>Message_sut * MSG_Receive(u8 task);
<br>u8 MSG_Free(Message_sut* pMessage);
<br>每一个线程都有他自己的消息队列，消息会根据你需要发送的数据自动分配和释放内存。
</p>
