#include <iostream>
#include <libusb.h>
#include <stdio.h>
#include <string>

// Объявление функции printdev, которая выводит информацию об устройстве
void printdev(libusb_device *dev);
std::string getDeviceType(uint8_t desc);

int main()
{
       libusb_device **devs;           // Указатель на массив указателей на libusb_device (каждый элемент — устройство)
       libusb_context *context = NULL; // Контекст для работы с состоянием устройства при помощи libusb
       int r;                          // Переменная для хранения кодов возврата функций libusb
       ssize_t cnt;                    // Количество найденных устройств

       // Инициализация библиотеки libusb.  Возвращает 0 при успехе, отрицательное число — при ошибке.
       r = libusb_init(&context);
       if (r < 0)
       {
              std::cerr << "Ошибка: Инициализация libusb не удалась, код: " << r << std::endl;
              return 1;
       }

       libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, 3); // задать уровень подробности отладочных сообщений

       // Получение списка устройств.  Заполняет devs указателями на найденные устройства и возвращает их количество.
       cnt = libusb_get_device_list(context, &devs);
       if (cnt < 0)
       {
              std::cerr << "Ошибка: Не удалось получить список устройств, код: " << cnt << std::endl;
              libusb_exit(context);
              return 1;
       }

       std::cout << "Найдено устройств: " << cnt << std::endl;
       for (ssize_t i = 0; i < cnt; i++)
       {
              std::cout << "Устройство " << i + 1 << ":" << std::endl;
              printdev(devs[i]); // Вызов функции для вывода информации о каждом устройстве
       }

       libusb_free_device_list(devs, 1); // Освобождение памяти, выделенной для списка устройств (1 - освобождать сами устройства)
       libusb_exit(context);             // Закрытие контекста libusb
       return 0;
}

void printdev(libusb_device *dev)
{
       libusb_device_descriptor desc; // Структура для хранения дескриптора устройства
       int r;

       r = libusb_get_device_descriptor(dev, &desc); // Получение дескриптора устройства, который записывается в desc, где dev - устройство
       if (r < 0)
       {
              std::cerr << "Ошибка: Не удалось получить дескриптор устройства, код: " << r << std::endl;
              return;
       }

       std::cout << "Код класса устройства: " << (int)desc.bDeviceClass << std::endl;
       std::cout << "Класс устройства: "<<getDeviceType(desc.bDeviceClass) << std::endl;
       std::cout << "ID производителя: 0x" << std::hex << desc.idVendor << std::dec << std::endl;
       std::cout << "ID продукта: 0x" << std::hex << desc.idProduct << std::dec << std::endl;

       // Получение и вывод серийного номера
       libusb_device_handle *handle = nullptr;
       r = libusb_open(dev, &handle); // Открытие устройства для доступа к строковому дескриптору
       if (r != 0)
       {
              std::cerr << "Ошибка: Не удалось открыть устройство, код: " << r << std::endl;
              std::cout << "--------------------" << std::endl;
              return;
       }

       unsigned char serialNumber[256]; // Буфер для хранения серийного номера
       r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serialNumber, sizeof(serialNumber)); 
       if (desc.iSerialNumber == 0){
              std::cout << "Серийный номер не доступен." << std::endl;
       }
       else if (r > 0)
       {
              std::cout << "Серийный номер: " << (char *)serialNumber << std::endl;
       }

       else{
              std::cerr << "Ошибка: Не удалось получить серийный номер, код: " << r << std::endl;
              std::cerr << libusb_strerror(r) << std::endl; //выводит сообщение о типе ошибки
       }
       libusb_close(handle);
       std::cout << "--------------------" << std::endl;
}
std::string getDeviceType(uint8_t desc){
       switch (desc){
       case 0x00:return "Код отсутствует (информацию о классе нужно получать в дескрипторе интерфейса)";
       case 0x01:return "Аудиоустройство (если код получен из дескриптора интерфейса, а не устройства)";
       case 0x02:return "Коммуникационное устройство (сетевой адаптер)";
       case 0x03:return "Устройство пользовательского интерфейса";
       case 0x05:return "Физическое устройство";
       case 0x06:return "Изображения";
       case 0x07:return "Принтер";
       case 0x08:return "Устройство хранения данных";
       case 0x09:return "Концентратор";
       case 0x0A:return "CDC-Data";
       case 0x0B:return "Smart Card";
       case 0x0D:return "Content Security";
       case 0x0E:return "Видеоустройство";
       case 0x0F:return "Персональное медицинское устройство";
       case 0x10:return "Аудио- и видеоустройства";
       case 0xDC:return "Диагностическое устройство";
       case 0xE0:return "Беспроводной контроллер";
       case 0xEF:return "Различные устройства";
       default:return "Специфическое устройство";
       }
}