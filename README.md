Советы по использованию
Множество типов при генерации могут вызвать проблемы с памятью, поскольку ваш стек может не выдержать большого объема данных. Рекомендуется использовать не более трех типов или просто увеличить размер стека.
Типы с маленькой точностью могут не работать корректно, но это уже зависит от реализации физики.
Слишком большие типы также могут привести к сбоям программы, снова из-за особенностей начальной реализации.

---
Скачивание из файла работает, можно даже считать поле другого размера, но там есть какие-то рамки на изменения.
Сохраннение в файл работает каждый тик, на каком типке это будет происходить передается в метод класса в через main, сколько будет тиков тоже передается в методк класса через main.
Считывание происходит из input.json, сохраняется в outpu.json
---

Программу можно запустить с произвольными типами через `cmake -DTYPES=типы ..` символы () нужно либо экранировать через \, либо писать как строку, используя кавычки.
При работе программы памяти хватает на комбинации из 2х типов, чтобы использовать большее количество комбинаций - нужно прописать команду `ulimit -s bytes` при запуске. CMake такой возможности не предоставляет, а bash требует прав.
Далее при запуске программы типы выбираются так же как и в ТЗ. Программа работает без потери производительности
---

