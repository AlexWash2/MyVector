#ifndef VECTOR_H
#define VECTOR_H
#include <iostream>
#include <limits>

template <typename T>

class MyVector{
  private:
    T* arr;

    // лучше использовать не int, а size_t, потому что size_t достаточно вместителен для любых значений (при работе с большим capacity это очень важно!)
    size_t size; // реальное количество элементов

    size_t capacity; // текущий размер контейнера // Предыстория создания capacity: до этого у меня был только size и мнимое представление по привычке, что в случае чего каким-то магическим образом size может меняться сам.
                  // Однако, конечно это на самом деле не так, вектор это лишь илюзия саморасширяющегося контейнера, на самом деле это по сути такой же массив, но динамический, а стало быть мы вольны сами контролировать
                  // выделение памяти. Так я и сделал, одна переменная (size) ведет учет данных (сколько их), а другая (capacity) отслеживает сколько физически места есть в массиве для этих переменных. Таким образом, с помощью capacity
                  // я предусматриваю алгоритм переаллокации памяти (расширения) моего пользовательского контейнера MyVector, что для пользователя может выглядеть как "бездонная бочка".

    bool ChangeCapacity(const size_t& amount){ // функция для учета емкости моего вектора
        if(capacity == 0){ capacity += 1;}
        if(size + amount > capacity){ 
            while(size + amount > capacity){capacity *= 2; }
            return true; // надо расширять
        }
        else{
            return false; // расширение не трубуется
        }
    }

  public:

    MyVector(size_t size): size(size), capacity(size) {
        arr = new T[capacity]; // инициализация значениями по умолчанию (0)
        std::cout << "vector has been created" << '\n';
        } // параметрический конструктор
    
    MyVector(): size(0), capacity(0) {
        arr = new T[capacity];
        std::cout << "default vector has been created" << '\n'; 
    } // конструктор по умолчанию

    // Деструктор необходим т.к. мы работаем с динамической памятью, здесь он особенно важен
    ~MyVector() {
        delete[] arr;
    }

    // копирование (правило 3) 

    MyVector(MyVector &other): arr(new T[other.size]), size(other.size), capacity(other.capacity) {
        for(size_t i = 0; i < size; i++){
            arr[i] = other.arr[i];
        }
    } // конструктор копирования
             
    MyVector& operator=(MyVector &other){  
        if(this != &other){ // учитываем случаи самоприсваивания 
            delete[] arr;  // освобождаем старый массив
            size = other.size;
            capacity = other.capacity;
            arr = new T[capacity];   // выделяем новый массив
            for(size_t i = 0; i < size; i++){
                arr[i] = other.arr[i];                 
            }
        }
        return *this; // возвращаем указатель на текущий объект
    }

    // ==================================================================================================================
    // работаем над списком инициализации

    void operator=(std::initializer_list<T> ilist){
        size_t amount = ilist.size();
        this -> ChangeCapacity(amount);
        T* new_arr = new T[capacity];

        auto iit = ilist.begin();
        for(size_t i = 0; i < amount; i++){
            new_arr[i] = *(iit+ i);
        }

        delete[] arr; // Старый блок удаляется
        arr = new_arr; // А тут мы перенаправляем наш основной указатель arr на новую память. Т.к. теперь мы будем работать с обновленным динамическим массивом
        size = amount;
    }

    MyVector(std::initializer_list<T> ilist): size(ilist.size()), capacity(ilist.size()) {
        arr = new T[capacity]; // не забываем! иначе будет: Segmentation fault
        auto iit = ilist.begin();
        for(size_t i = 0; i < size; i++){
            arr[i] = *(iit + i); 
        }
    }

    // правило пяти:

    MyVector(MyVector&& other): arr(other.arr), size(other.size), capacity(other.capacity) {
        other.arr = nullptr; // освобождаем ресурсы исходного объекта, после перемещения его состояния в новый объект
        other.size = 0;
        other.capacity = 0;
    }

    MyVector& operator=(MyVector&& other){
    if (this != &other) {
        delete[] arr;
        arr = other.arr; // Перемещаем указатель
        size = other.size;
        capacity = other.capacity;
        other.arr = nullptr; // Обнуляем 
        other.size = 0;
        other.capacity = 0;
    }
    return *this; // возвращаем указатель на новосозданный объект
}

    // основной функционал

    size_t MySize() { return size; }
    size_t MySize()const{return size;}
    size_t MyCapacity() { return capacity; }
    size_t MyCapacity()const { return capacity; }

    size_t My_max_size() { return std::numeric_limits<size_t>::max() / sizeof(T); }
    size_t My_max_size() const{ return std::numeric_limits<size_t>::max() / sizeof(T); }
    // пояснение: 
    // size_t - беззнаковый целочисленный тип. Он гарантированно достаточно велик, чтобы содержать размер любого объекта в памяти, 
    // что делает его предпочтительным для нас, так как мы хотим вычислить максимальное количество элементов типа T, которые могут быть размещены в памяти.
    // сама логика вычисления:
    // std::numeric_limits<size_t>::max() - возвращает максимальное значение, которое может быть занято в памяти типом size_t.
    // sizeof(T) - возвращает размер одного элемента типа T в байтах (в зависимости от типа нашего вектора)
    // std::numeric_limits<size_t>::max() / sizeof(T) - делим максимальное количество байтов, которое мы можем занимать наибольшим типом size_t на размер одного элемента типа T (тип нашего вектора), 
    // чтобы получить максимальное количество элементов типа T, которые могут быть одновременно размещены в доступной памяти.
    /* 
    Например, для архитектуры VScode:
        1. максимальное значение, которое может хранить size_t = 1.8446744e+19
        2. если в нашем векторе: T => int (т.е.  MyVector<int> vec), то максимальное количество элементов типа T (int) = 1.8446744e+19 / 4 = 4.611686e+18 (ну +-)
        (P.S. делим на 4, т.к. int = 4 байта)
    */

    void My_push_back(const T& value){
        bool check = this -> ChangeCapacity(1); // вызываю для 1 элемента
        if(check){
            T* new_arr = new T[capacity];
            for(size_t i = 0; i < size; i++){
                new_arr[i] = arr[i];
            }
            new_arr[size] = value;
            delete[] arr;  // прежнюю версию массива удаляем
            arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
            size++;
        }
        else{ // если capacity уже достаточно велик => переаллокация не требуется
            arr[size] = value;
            size++;
        }
    }

    T& operator[](const size_t& index){
        if (index < 0 || index >= size) { // index < 0 || index >= size => index < 0 не имеет смысла, т.к. index имеет беззнаковый тип size_t (поскольку это не мешает оставлю как есть)
            std::cout << "Неверный индекс!" << '\n';
        }
        return arr[index]; 
    }
    const T& operator[](const size_t& index)const{
        if (index < 0 || index >= size) { // index < 0 || index > size => index < 0 не имеет смысла, т.к. index имеет беззнаковый тип size_t (поскольку это не мешает оставлю как есть)
            std::cout << "Неверный индекс!" << '\n';
        }
        return arr[index];
    }

    T& My_at(const size_t& index){
        if (index < 0 || index >= size) {
            std::cout << "Неверный индекс!" << '\n';
        }
        return arr[index]; // Здесь мы возвращаем именно ссылку на элемент, чтобы избежать создания дубликатов и обеспечить корректный функционал, ведь если используя [] или .at() мы хотим изменить элемент на выбранной позиции index. Так вот, если [] или .at() будут возвращать копию элемента, то мы сможем изменить только ее, эти изменения не каснутся элемента на позиции index в нашем векторе,
                           // он не изменится. А вот если [] или .at() будут возвращать ссылку на нужный нам элемент (по сути это и есть сам элемент на позиции index в нашем векторе), то мы получим прямой доступ к нему и сможем провести все изменения над ним, как и планировали
    }
    const T& My_at(const size_t& index)const{
        if (index < 0 || index >= size) {
            std::cout << "Неверный индекс!" << '\n';
        }
        return arr[index]; // Здесь мы возвращаем именно ссылку на элемент, чтобы избежать создания дубликатов и обеспечить корректный функционал, ведь если используя [] или .at() мы хотим изменить элемент на выбранной позиции index. Так вот, если [] или .at() будут возвращать копию элемента, то мы сможем изменить только ее, эти изменения не каснутся элемента на позиции index в нашем векторе,
                           // он не изменится. А вот если [] или .at() будут возвращать ссылку на нужный нам элемент (по сути это и есть сам элемент на позиции index в нашем векторе), то мы получим прямой доступ к нему и сможем провести все изменения над ним, как и планировали
    }

    T& My_front(){return arr[0];}
    const T& My_front()const{return arr[0];}

    T& My_back(){return arr[size - 1];}
    const T& My_back()const{return arr[size - 1];}

    T* My_data(){return &arr[0]; } // возвращаем указатель на начало нашего массива (его первый элемент)
    const T* My_data()const{return &arr[0]; }

    // ================================================================================
  void MyClear(){ 
        if(capacity == 0){ // не перевыделяем память лишний раз, если чистить нечего
            std::cout << "Массив уже пуст, чистить нечего!" << '\n';
            return;
        }
        delete[] arr;  // очистка (если все же есть, что очищать)

       // по сути бесмыслено, указатель уже точно не будет висячим, тк мы переназначим его на обновленную область памяти => nullptr здесь не нужен (хотя и не вреден).
       // arr = nullptr; // (предотвращаем возникновение висячего указателя, который после очистки памяти с помощью delete[], будет указывать на пустую очищенную область памяти)

        size = 0; // нет элементов => size = 0
        // capacity = 0; // capacity не меняем - это очень важно, на его основе будет образован новый массив

        // arr = new T[size]; // вызывало Segmentation fault! Дело в том, что ведь мы в методе clear не меняем capacity, т.е. если например изначально в векторе было 5 элементов, то после применения метода clear к этому вектору
                              // его size станет 0, НО capacity останется 5. А далее проблема возникает из-за того, что мы инициализировали новую область памяти используя size, который равен нулю, и получается что мы как бы проинициализировали 
                              // новый массив с нулевым размером, хотя сами же сохранили capacity = 5 - размер который вроде как ожидается. Т.е. мы как бы обещаем размер 5, а создаем массив с размером 0 => это приводит к ошибкам!
        arr = new T[capacity]; // начинаем все с чистого листа и правильно создаем новый массив, опираясь на capacity
    }

    void My_pop_back(){
        if(size > 0){
            T* new_arr = new T[size-1]; // здесь new_arr - это указатель на тип T. Это локальная переменная внутри моей функции My_pop_back(). Т.е. она создается при входе в функцию и уничтожается при выходе из нее.
                                        // А в части: new T[size - 1] мы выделяем память для массива из size - 1 элементов типа T. Дальше продолжаем работать с новым динамическим массивом

            for(size_t i = 0; i < size - 1; i++){ // для копирования данных (если есть, что копировать)
                new_arr[i] = arr[i];
            }
            delete[] arr;
            arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта, т.к. arr теперь указывает на тот же участок динамической памяти, на который указывал new_arr.
            size--;
        }
        else{
            std::cout << "Нельзя что-то удалить из пустого вектора!" << '\n';
            return;
        }
    } // материал для понимания принципа работы функции My_pop_back() :
/*
new_arr - это локальная переменная, существующая только в пределах функции My_pop_back(). После завершения функции new_arr перестает существовать (уничтожается).
arr - это член класса MyVector, который существует на протяжении всего времени существования объекта MyVector.

Внутри My_pop_back():

1. Мы создаем новый массив new_arr, копируем в него данные из старого массива arr (за исключением последнего элемента).
2. Удаляем старый массив, на который указывает arr (delete[] arr). После этого arr становится висячим указателем (указывает на уже освобожденную память и не может быть использован).
3. Перенаправляем arr на new_arr (arr = new_arr). Теперь arr указывает на новый массив, и MyVector продолжает корректно работать с данными.
4. Когда My_pop_back() завершает работу, new_arr автоматически уничтожается. Но это не проблема, потому что адрес, на который указывал new_arr, уже сохранен в arr, и мы можем продолжать использовать новый массив через arr.

Таким образом, arr продолжает "владеть" актуальным массивом данных на протяжении всего времени жизни объекта MyVector. Функция My_pop_back() временно использует new_arr для создания нового массива, но в конечном итоге переносит ответственность за управление памятью обратно на arr.
*/

    // Вывод:
    /* Т.е. в функции My_pop_back() мы в начале создаем новый указатель new_arr (он является локальным) на динамическую память типа T и инициализируем этот указатель новой выделенной памятью через new T (пока эта память пуста). 
       Затем мы заролняем эту новую память, копируя в нее элементы из старой пямяти (старого массива) arr, все это происходит в циклн for. После завершения процесса копирования из старого массива arr в 
       новый new_arr (или из старой памяти в новую выделенную), мы удаляем (очищаем) старый массив arr. Но после удаления старой памяти, наш прежний указатель arr (глобальный, т.к. является полем класса), который 
       указывал на нее, станет висячим и это может быть опасно, ведь после завершения тела нашей функции, наш новый указатель new_arr уничтожится и тогда мы потеряем доступ к нашей новой динамической памяти - произойдет
       утечка памяти, что очень опасно. Поэтому, чтобы предусмотреть это, мы заранее перенаправляем наш глобальный указатель arr на новую динамическую память, таким образом он перестает быть висячим, и после завершения тела
       функции, когда локальный указатель new_arr уничтожится, мы не потеряем доступ к нашей новой динамической памяти, ведь на нее будет указывать глобальный указатель arr.*/

// ====================================================================================

/*
void MyResize(const size_t& count){
    if(size > count){ // если новый размер меньше
        T* new_arr = new T[count];
        for(size_t i = 0; i < count; i++){ // для копирования данных 
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
        size = count; // обновляем размер
    }
    else if(size < count){ // если новый размер больше
        T* new_arr = new T[count];
        for(size_t i = 0; i < size; i++){ // для копирования данных 
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
        for(size_t i = size; i < count; i++){ // а оставшееся место заполняем нулями
            this->My_push_back(0);
        }
    }
}
*/ // доработанная версия внизу...

void MyResize(const size_t& count){
    if(size > count){ // если новый размер меньше
        T* new_arr = new T[count];
        for(size_t i = 0; i < count; i++){ // для копирования данных 
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
        size = count; // обновляем размер
    }
    else if(size < count){ // если новый размер больше
        bool check = this -> ChangeCapacity(count);
        if(check){
            T* new_arr = new T[capacity];
            for(size_t i = 0; i < size; i++){ // для копирования данных 
                new_arr[i] = arr[i];
            }
            for(size_t i = size; i < count; i++){ // а оставшееся место заполняем по умолчанию
                new_arr[i] = T(); 
            }
            delete[] arr;
            arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
            size = count;
        }
        else{
            for(size_t i = size; i < count; i++){ // свободное место заполняем по умолчанию
                arr[i] = T(); 
            }
            size = count;
        }
    }
    else if(size == count){ return; }
}
    // вторая реализация

/*
void MyResize(const size_t& count, const T& value){
    if(size > count){
        T* new_arr = new T[count];
        for(size_t i = 0; i < count; i++){ // для копирования данных 
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
        size = count;
    }
    else if(size < count){
        T* new_arr = new T[count];
        for(size_t i = 0; i < size; i++){ // для копирования данных 
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
        for(size_t i = size; i < count; i++){ // а оставшееся место заполняем value
            this->My_push_back(value);
        }
    }      
} */

void MyResize(const size_t& count, const T& value){
    if(size > count){ // если новый размер меньше
        T* new_arr = new T[count];
        for(size_t i = 0; i < count; i++){ // для копирования данных 
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
        size = count; // обновляем размер
    }
    else if(size < count){ // если новый размер больше
        bool check = this -> ChangeCapacity(count);
        if(check){
            T* new_arr = new T[capacity];
            for(size_t i = 0; i < size; i++){ // для копирования данных 
                new_arr[i] = arr[i];
            }
            for(size_t i = size; i < count; i++){ // а оставшееся место заполняем value
                new_arr[i] = value;
            }
            delete[] arr;
            arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив актуальным для нашего текущего объекта
            size = count;
        }
        else{
            for(size_t i = size; i < count; i++){ // свободное место заполняем по умолчанию
                arr[i] = value; 
            }
            size = count;
        }
    }
    else if(size == count){ return; }
}



bool MyEmpty(){
    if(size == 0){ return true; }
    else{ return false; }
}

bool MyEmpty()const{
    if(size == 0){ return true; }
    else{ return false; }
}

// реализуем класс итераторов

class MyIterator {
    private:
        MyVector* vector;
        size_t index;
    public:
        MyIterator(MyVector* vec, size_t id) : vector(vec), index(id) {}
        ~MyIterator(){}

        //T& operator* (){ return (*vector)[index];}
        //T& operator [](size_t id){ return (*vector)[index + id];}

        T& operator* (){
            if(index < 0 || index >= vector->MySize()){std::cout << "Неверный итератор!" << '\n';}
            return (*vector)[index];
        }
        T& operator [](size_t id){ 
            if(index < 0 || index + id >= vector->MySize()){std::cout << "Неверный итератор!" << '\n';}
            return (*vector)[index + id];
        }

        MyIterator& operator++() {index++; return *this; }
        MyIterator operator++(int) {MyIterator tmp = *this; ++(*this); return tmp; }  // Постфиксный ++

        MyIterator& operator--() {index--; return *this; }
        MyIterator operator--(int) {MyIterator tmp = *this; --(*this); return tmp; }

        size_t get_index(){ return index;}
        size_t get_upper_bound(){return vector->MySize();}

        MyIterator operator+(size_t add) {return MyIterator(this->vector, index + add);}
        MyIterator operator-(size_t add) {return MyIterator(this->vector, index - add);}

        MyIterator& operator +=(size_t add) {index += add; return *this; }
        MyIterator& operator -=(size_t add) {index -= add; return *this; }

       // size_t operator+(MyIterator& other) {return index + other.index;} // нужен ли ?
        int operator-(MyIterator& other) {
            if(index < other.index) { std::cout << "Нельзя из меньшего итератора вычитать больший!" << '\n';}
            return index - other.index;
        } // оставлю int, с расчетом на то, что разница может быть отрицательной при неверном вычитании итераторов

        /*bool operator!=(MyIterator& other) { 
            if(vector == other.vector){ 
                return index != other.index; 
            }
            else{
                std::cout << "Нельзя сравнивать итераторы 2х разных векторов!"
                return;
            }
        }*/ // пустое возвращение для функции bool => ошибка!

        bool operator !=(MyIterator& other){
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return true; // разные векторы => итераторы действительно не могут быть равными
            }
            return index != other.index; 
        }

        bool operator ==(MyIterator& other){
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // разные векторы => итераторы действительно не могут быть равными
            }
            return index == other.index; 
        }

        bool operator <(MyIterator& other){
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // разные векторы => итераторы действительно не могут быть равными
            }
            return index < other.index; 
        }

        bool operator <=(MyIterator& other){
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // разные векторы => итераторы действительно не могут быть равными
            }
            return index <= other.index; 
        }

        bool operator >(MyIterator& other){
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // разные векторы => итераторы действительно не могут быть равными
            }
            return index > other.index; 
        }

        bool operator >=(MyIterator& other){
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // разные векторы => итераторы действительно не могут быть равными
            }
            return index >= other.index; 
        }  

};

    MyIterator begin() { return MyIterator(this, 0); }
    MyIterator end() { return MyIterator(this, size);}

    /* // Чтобы for (auto x : container) работал с const MyVector, нужны const-версии begin() и end()!!!
    MyIterator begin() const {return MyIterator(this, 0); }
    MyIterator end() const {return MyIterator(this, size);}
    */

    /*
    MyIterator My_begin() { return MyIterator(this, 0); }
    MyIterator My_end() { return MyIterator(this, size);}
    */ // Проблема в названии: при работе с циклом типа for (auto x : container) компилятор распознает только названия begin() и end().

/* // наследие прошлого
T* My_begin(){
    return &arr[0];
}

T* My_end(){
    return &arr[size];  // именно size, а не size-1, потому что итератор в конце должен указывать на элемент после последнего элемента, т.е. он окажется за пределами нашего массива (поэтому потом его нужно будет вернуть)
}
*/

// КЛАСС КОНСТАНТНЫХ ИТЕРАТОРОВ

class MyConstIterator {
    private:
        const MyVector* vector; // указатель на константный вектор
        size_t index; // сам индекс менять можно, стандартный вектор в С++ запрещает менять данные контейнера, но позволяет перемещаться по ним => мы можем менять состояние index
    public:
        MyConstIterator(const MyVector* vec, size_t id) : vector(vec), index(id) {}
        ~MyConstIterator(){}

        //const T& operator* ()const{ return (*vector)[index];}
        //const T& operator [](size_t id)const{ return (*vector)[index + id];}

        const T& operator* ()const{
            if(index < 0 || index >= vector->MySize()){std::cout << "Неверный итератор!" << '\n';}
            return (*vector)[index];
        }
        const T& operator [](size_t id)const{ 
            if(index < 0 || index + id >= vector->MySize()){std::cout << "Неверный итератор!" << '\n';}
            return (*vector)[index + id];
        }

        MyConstIterator& operator++() {index++; return *this; }
        MyConstIterator operator++(int) {MyConstIterator tmp = *this; ++(*this); return tmp; }  // Постфиксный ++

        MyConstIterator& operator--() {index--; return *this; }
        MyConstIterator operator--(int) {MyConstIterator tmp = *this; --(*this); return tmp; }

        size_t get_index()const{ return index;}
        size_t get_upper_bound()const{return vector->MySize();}

        MyConstIterator operator+(size_t add) const {return MyConstIterator(this->vector, index + add);}
        MyConstIterator operator-(size_t add) const {return MyConstIterator(this->vector, index - add);}

        MyConstIterator& operator +=(size_t add) {index += add; return *this; }
        MyConstIterator& operator -=(size_t add) {index -= add; return *this; }

        // size_t operator+(const MyConstIterator& other)const {return index + other.index;} // нужен ли ?
        int operator-(const MyConstIterator& other)const {
            if(index < other.index) { std::cout << "Нельзя из меньшего итератора вычитать больший!" << '\n';}
            return index - other.index;
        } // оставлю int, с расчетом на то, что разница может быть отрицательной при неверном вычитании итераторов

        // реляционные операторы const-версии:

    bool operator !=(const MyConstIterator& other)const{
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return true; // костыль??? // разные векторы => итераторы действительно не могут быть равными
            }
            return index != other.index; 
        }

        bool operator ==(const MyConstIterator& other)const{
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // костыль??? // разные векторы => итераторы действительно не могут быть равными
            }
            return index == other.index; 
        }

        bool operator <(const MyConstIterator& other)const{
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // костыль??? // разные векторы => итераторы действительно не могут быть равными
            }
            return index < other.index; 
        }

        bool operator <=(const MyConstIterator& other)const{
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // костыль??? // разные векторы => итераторы действительно не могут быть равными
            }
            return index <= other.index; 
        }

        bool operator >(const MyConstIterator& other)const{
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // костыль??? // разные векторы => итераторы действительно не могут быть равными
            }
            return index > other.index; 
        }

        bool operator >=(const MyConstIterator& other)const{
            if(vector != other.vector){
                std::cout << "Нельзя сравнивать итераторы двух разных векторов!" << '\n';
                return false; // костыль??? // разные векторы => итераторы действительно не могут быть равными
            }
            return index >= other.index; 
        }

};

    MyConstIterator begin() const { return MyConstIterator(this, 0); }
    MyConstIterator end() const { return MyConstIterator(this, size);}


void MyErase(MyIterator it){
    // int index = std::distance(arr, it); // наследие прошлого
    size_t index = it.get_index(); // улучшенный вариант с использованием класса итераторов
    if(index < 0 || index >= size){ 
        std::cout << "Ты выходишь за пределы массива!" << '\n';
        return;
    }
    else{  
        T* new_arr = new T[size-1];
        for(size_t i = 0; i < index; i++){ // для копирования данных (1 половина, до удаленного элемента)
            new_arr[i] = arr[i];
        }
        for(size_t i = index + 1; i < size; i++){ // для копирования данных (2 половина, после удаленного элемента)
            new_arr[i-1] = arr[i]; // учитываем что прежний массив был меньше на -1
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив (новую выделенную динамическую память) актуальным для нашего текущего объекта
        size--; // корректируем размер
    }
}

void MyErase(MyIterator it_begin, MyIterator it_end){ // + проверка что индекс начала меньше конца!
    // int index_begin = std::distance(arr, it_begin);
    // int index_end = std::distance(arr, it_end);
    size_t index_begin = it_begin.get_index();
    size_t index_end = it_end.get_index();
    int diff = it_end - it_begin; 
    if(diff <= 0) {return;} // при попытке вычесть из меньшего итератора больший завершаем работу метода / или если диапозон нулевой
    if((index_begin < 0 || index_begin > size) || (index_end < 0 || index_end > size)){
        std::cout << "Ты выходишь за пределы массива!" << '\n';
        return;
    }
    else{  
        T* new_arr = new T[size - diff];
        for(size_t i = 0; i < index_begin; i++){ // для копирования данных (1 половина, до удаленного диапозона элементов)
            new_arr[i] = arr[i];
        }
        for(size_t i = index_end; i < size; i++){ // для копирования данных (2 половина, после удаленного диапозона элементов) + нужно учесь что итератор на позиции end() смотрит не на последний элемент массива, а на область за массивом!
            new_arr[i-diff] = arr[i]; // учитываем что прежний массив был меньше 
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив (новую выделенную динамическую память) актуальным для нашего текущего объекта
        size -= diff; // корректируем размер
    }
}

void MyInsert(MyIterator it, const T& value){ 
    // int index = std::distance(arr, it); // расчет индекса для новго элемента
    size_t index = it.get_index(); 
    if(index < 0 || index > size){
        std::cout << "Ты выходишь за пределы массива!" << '\n';
        return;
    }
    else{
        this -> ChangeCapacity(1);
        T* new_arr = new T[capacity];
        for(size_t i = 0; i < index; i++){ // для копирования данных (1 половина, до нового элемента)               
            new_arr[i] = arr[i];
        }
        new_arr[index] = value; // вставляем новый элемент на желаемую позицию
        for(size_t i = index + 1; i < size + 1; i++){ // для копирования данных (2 половина, после нового элемента)
            new_arr[i] = arr[i-1]; // учитываем что прежний массив был меньше на -1
        }
        delete[] arr;
        arr = new_arr; // Устанавливаем arr указывать на new_arr, что делает новый массив (новую выделенную динамическую память) актуальным для нашего текущего объекта
        size++; // увеличится с учетом добавления нового элемента
    }
}

void MyInsert(MyIterator it, const size_t& amount, const T& value) {
    // int index = std::distance(arr, it);
    size_t index = it.get_index(); 
    if (index < 0 || index > size) {
        std::cout << "Ты выходишь за пределы массива!" << '\n';
        return;
    }
    else{
    this->ChangeCapacity(amount); 
    T* new_arr = new T[capacity];

    for (size_t i = 0; i < index; i++) {
        new_arr[i] = arr[i];
    }

    for (size_t i = 0; i < amount; i++) {
        new_arr[index + i] = value;
    }

    for (size_t i = index + amount; i < size + amount; i++){
            new_arr[i] = arr[i-amount]; 
    }

    delete[] arr;
    arr = new_arr;
    size += amount;
    }
} 

 //ОТДЕЛЬНО РАЗБОР ПРОБЛЕМЫ ВЗАИМОДЕЙСТВИЕ INSERT (И ДРУГИХ МЕТОДОВ) С ИТЕРАТОРАМИ

// Объяснение:
/*
Мы начали с реализации метода MyInsert(), который использовал указатели в качестве итераторов.
Основная проблема в том, что при изменении вектора (например, при добавлении элементов, которое может вызвать переаллокацию памяти)
указатели становятся недействительными. Это происходит потому, что переаллокация выделяет новый блок памяти, копирует туда старые данные
и удаляет старый блок. Указатели, которые указывали на старый блок, становятся висячими, и их использование ведет к неопределенному поведению.
*/

void MyInsert(MyIterator it, MyIterator other_it_begin, MyIterator other_it_end){
    size_t index = it.get_index();
    // int amount = std::distance(other_it_begin, other_it_end);
    int amount = other_it_end - other_it_begin;
    if(amount <= 0){ return;}  // если вставлять нечего
    if (index < 0 || index > size) {
        std::cout << "Ты выходишь за пределы массива, куда собираешься добавить новые данные!" << '\n';
        return;
    }
    else{
        size_t index_begin = other_it_begin.get_index();
        size_t index_end = other_it_end.get_index();
        if((index_begin < 0 || index_begin > other_it_begin.get_upper_bound()) || (index_end < 0 || index_end > other_it_end.get_upper_bound())){
            std::cout << "Ты выходишь за пределы массива, из которого хочешь взять элементы для вставки!" << '\n';
            return;
        }
        else{        
            this->ChangeCapacity(amount);
            T* new_arr = new T[capacity]; // Выделяем новый блок памяти

            for (size_t i = 0; i < index; i++) {
                new_arr[i] = arr[i];
            }
            for(size_t i = 0; i < amount; i++){
                new_arr[index + i] = *(other_it_begin+ i);
            }
            for(size_t i = index + amount; i < size + amount; i++){
                new_arr[i] = arr[i - amount];
            }
            delete[] arr; // Старый блок удаляется
            arr = new_arr; // А тут мы перенаправляем наш основной указатель arr на новую память. Т.к. теперь мы будем работать с обновленным динамическим массивом
            size += amount;
        }
    }
}

void MyInsert(MyIterator it, std::initializer_list<T> ilist){
    // int index = std::distance(arr, it); // расчет индекса для новго элемента
    size_t index = it.get_index();
    if(index < 0 || index > size){
        std::cout << "Ты выходишь за пределы массива!" << '\n';
        return;
    }
    else{
        size_t amount = ilist.size();
        this -> ChangeCapacity(amount);
        T* new_arr = new T[capacity];

        for (size_t i = 0; i < index; i++) {
            new_arr[i] = arr[i];
        }

        auto iit = ilist.begin();
        for(size_t i = 0; i < amount; i++){
            new_arr[index + i] = *(iit+ i);
        }

        for(size_t i = index + amount; i < size + amount; i++){
            new_arr[i] = arr[i - amount];
        }
        delete[] arr; // Старый блок удаляется
        arr = new_arr; // А тут мы перенаправляем наш основной указатель arr на новую память. Т.к. теперь мы будем работать с обновленным динамическим массивом
        size += amount;
    }
}

// реализация assign

void MyAssign(size_t count, const T& value){
    this -> ChangeCapacity(count);
    T* new_arr = new T[capacity];

    for(size_t i = 0; i < count; i++){
        new_arr[i] = value;
    }
    delete[] arr; // Старый блок удаляется
    arr = new_arr; // А тут мы перенаправляем наш основной указатель arr на новую память. Т.к. теперь мы будем работать с обновленным динамическим массивом
    size = count;  
}

void MyAssign(MyIterator other_it_begin, MyIterator other_it_end){
    size_t index_begin = other_it_begin.get_index();
    size_t index_end = other_it_end.get_index();
    int amount = other_it_end - other_it_begin;
    if(amount <= 0){ return;} 
    if((index_begin < 0 || index_begin > other_it_begin.get_upper_bound()) || (index_end < 0 || index_end > other_it_end.get_upper_bound())){
            std::cout << "Ты выходишь за пределы массива, из которого хочешь взять элементы для вставки!" << '\n';
            return;
        }
    else{
        this->ChangeCapacity(amount);
        T* new_arr = new T[capacity]; // Выделяем новый блок памяти

        for(size_t i = 0; i < amount; i++){
            new_arr[i] = *(other_it_begin + i);
        }
        delete[] arr; // Старый блок удаляется
        arr = new_arr; // А тут мы перенаправляем наш основной указатель arr на новую память. Т.к. теперь мы будем работать с обновленным динамическим массивом
        size = amount; 
    }
}

void MyAssign(std::initializer_list<T> ilist){
    size_t amount = ilist.size();
    this -> ChangeCapacity(amount);
    T* new_arr = new T[capacity]; // Выделяем новый блок памяти
    
    auto iit = ilist.begin();
    for(size_t i = 0; i < amount; i++){
        new_arr[i] = *(iit + i);
    }
    delete[] arr; // Старый блок удаляется
    arr = new_arr; // А тут мы перенаправляем наш основной указатель arr на новую память. Т.к. теперь мы будем работать с обновленным динамическим массивом
    size = amount;
}

void MySwap(MyVector& other){
    T* temp_ptr = arr;
    arr = other.arr;
    other.arr = temp_ptr;

    size_t temp_size = size;
    size = other.size;
    other.size = temp_size;

    size_t temp_capacity = capacity;
    capacity = other.capacity;
    other.capacity = temp_capacity;
}

void MyReserve(size_t new_cap){
    bool check = this -> ChangeCapacity(new_cap);
    if(check){
        T* new_arr = new T[capacity]; // Выделяем новый блок памяти
        for(size_t i = 0; i < size; i++){
            new_arr[i] = arr[i];
        }
        delete[] arr; // Старый блок удаляется
        arr = new_arr; 
    }
}

void My_shrink_to_fit(){
    bool check = this -> ChangeCapacity(0);
    if(!check){
        capacity = size;
        T* new_arr = new T[capacity];
        for(size_t i = 0; i < size; i++){
            new_arr[i] = arr[i];
        }
        delete[] arr; // Старый блок удаляется
        arr = new_arr;
    }
}

// ============================================================================================
// Реляционные операторы:

bool operator== (MyVector& other){
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    bool length_check = ((length_first == length_second)? true : false);

    if(length_check){
        bool flag = true;
        for(size_t i = 0; i < length_first; i++){
            if(this->arr[i] == other.arr[i]){
                flag = true;
            }
            else{
                flag = false;
                return flag;
            }
        }
        return flag;
    }
    else{
        return length_check;
    }
}
bool operator!= (MyVector& other){
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    bool length_check = ((length_first != length_second)? true : false);

    if(!length_check){
        bool flag = false; // равны (по умолчанию)
        for(size_t i = 0; i < length_first; i++){
            if(this->arr[i] != other.arr[i]){
                flag = true; // не равны
                return flag;
            }
            else{
                flag = false;
            }
        }
        return flag;
    }
    else{
        return length_check;
    }
}

/* // изначальная реализация != (менее эффективная, та что выше лучше)
bool operator!= (MyVector& other){
    int length_first = this->MySize();
    int length_second = other.MySize();
    bool length_check = ((length_first != length_second)? true : false);

    if(!length_check){
        bool flag = true; // не равны
        int count = 0; // КОСТЫЛЬ!
        for(int i = 0; i < length_first; i++){
            if(this->arr[i] == other.arr[i]){
                count++;
                flag = false;
            }
            else{
                flag = true;
            }
        }
        flag = (count == length_first)? false : true;
        return flag;
    }
    else{
        return length_check;
    }
}
*/


bool operator< (MyVector& other){
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] < other.arr[i]){ 
            flag = true; 
            return flag;
        }
        else{ 
            if(this->arr[i] > other.arr[i]){
                flag = false; 
                return flag;
            }
            flag = false;
        }
    }
    if(!flag){ // значит все элементы равны
        flag = ((length_first < length_second)? true : false);
    }
    return flag;
}
bool operator<= (MyVector& other){
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] < other.arr[i]){ 
            flag = true; 
            return flag; 
        }
        else{ 
            if(this->arr[i] > other.arr[i]){
                flag = false;
                return flag; 
            }
            flag = true;
        }
    }
    if(flag){ // значит все элементы равны
        flag = ((length_first <= length_second)? true : false);
    }
    return flag;
}


bool operator> (MyVector& other){
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] > other.arr[i]){ 
            flag = true; 
            return flag;
        }
        else{ 
            if(this->arr[i] < other.arr[i]){
                flag = false; 
                return flag;
            }
            flag = false;
        }
    }
    if(!flag){ // значит все элементы равны
        flag = ((length_first > length_second)? true : false);
    }
    return flag;
}
bool operator>= (MyVector& other){
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] > other.arr[i]){ 
            flag = true; 
            return flag; 
        }
        else{ 
            if(this->arr[i] < other.arr[i]){
                flag = false;
                return flag; 
            }
            flag = true;
        }
    }
    if(flag){ // значит все элементы равны
        flag = ((length_first >= length_second)? true : false);
    }
    return flag;
}

// Реляционные операторы (const):

bool operator== (MyVector& other)const{
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    bool length_check = ((length_first == length_second)? true : false);

    if(length_check){
        bool flag = true;
        for(size_t i = 0; i < length_first; i++){
            if(this->arr[i] == other.arr[i]){
                flag = true;
            }
            else{
                flag = false;
                return flag;
            }
        }
        return flag;
    }
    else{
        return length_check;
    }
}
bool operator!= (MyVector& other)const{
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    bool length_check = ((length_first != length_second)? true : false);

    if(!length_check){
        bool flag = false; // равны (по умолчанию)
        for(size_t i = 0; i < length_first; i++){
            if(this->arr[i] != other.arr[i]){
                flag = true; // не равны
                return flag;
            }
            else{
                flag = false;
            }
        }
        return flag;
    }
    else{
        return length_check;
    }
}


bool operator< (MyVector& other)const{
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] < other.arr[i]){ 
            flag = true; 
            return flag;
        }
        else{ 
            if(this->arr[i] > other.arr[i]){
                flag = false; 
                return flag;
            }
            flag = false;
        }
    }
    if(!flag){ // значит все элементы равны
        flag = ((length_first < length_second)? true : false);
    }
    return flag;
}
bool operator<= (MyVector& other)const{
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] < other.arr[i]){ 
            flag = true; 
            return flag; 
        }
        else{ 
            if(this->arr[i] > other.arr[i]){
                flag = false;
                return flag; 
            }
            flag = true;
        }
    }
    if(flag){ // значит все элементы равны
        flag = ((length_first <= length_second)? true : false);
    }
    return flag;
}


bool operator> (MyVector& other)const{
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] > other.arr[i]){ 
            flag = true; 
            return flag;
        }
        else{ 
            if(this->arr[i] < other.arr[i]){
                flag = false; 
                return flag;
            }
            flag = false;
        }
    }
    if(!flag){ // значит все элементы равны
        flag = ((length_first > length_second)? true : false);
    }
    return flag;
}
bool operator>= (MyVector& other)const{
    size_t length_first = this->MySize();
    size_t length_second = other.MySize();
    size_t length_final = ((length_first < length_second)? length_first : length_second); // сравниваем лексикографически по меньшему контейнеру

    bool flag = true; 
    for(size_t i = 0; i < length_final; i++){
        if(this->arr[i] > other.arr[i]){ 
            flag = true; 
            return flag; 
        }
        else{ 
            if(this->arr[i] < other.arr[i]){
                flag = false;
                return flag; 
            }
            flag = true;
        }
    }
    if(flag){ // значит все элементы равны
        flag = ((length_first >= length_second)? true : false);
    }
    return flag;
}

// ============================================================================================
// emplace

template<typename F>
void MyEmplace(MyIterator it, F&& value){
    size_t index = it.get_index();
    if (index < 0 || index > size) {
        std::cout << "Ты выходишь за пределы массива!" << '\n';
        return;
    } 
    else {
        this -> ChangeCapacity(1);
        T* new_arr = new T[capacity];
        for (size_t i = 0; i < index; i++) {
            new_arr[i] = std::move(arr[i]); // Перемещаем, а не копируем
        }
        new_arr[index] = T(std::forward<F>(value)); // Создаем объект на месте
        for (size_t i = index + 1; i < size + 1; i++) {
            new_arr[i] = std::move(arr[i - 1]); // Перемещаем, а не копируем
        }
        delete[] arr;
        arr = new_arr;
        size++;
    }
}

template<typename F>
T& MyEmplaceBack(F&& value){ 
    bool check = this -> ChangeCapacity(1);
    if(check){
        T* new_arr = new T[capacity];
        for (size_t i = 0; i < size; i++){
            new_arr[i] = std::move(arr[i]);
        }
        new_arr[size] = T(std::forward<F>(value));
        delete[] arr;
        arr = new_arr;
        size++;
        return arr[size - 1]; // возвращаем ссылку на новый элемент
    }
    else{
        arr[size] = T(std::forward<F>(value));
        size++;
        return arr[size - 1]; // возвращаем ссылку на новый элемент
    }
}


};


#endif

// TO CHECK

// 1. size_t или int везде ??? +
// 2. MyResize пересмотреть реализацию и потестить +
// 3. реляционные операторы const-версии и const-версии begin() и end() для итераторов, вообще нужны или нет, плюс потестить * и [] +
// 4. добавить проверку что индекс начала меньше конца (в таких методах, как: MyErase, MyInsert, MyAssign...) +
// 5. еще потестить!!! MyInsert 2 +
// 6. еще раз проверить erase и у аналогичных методов потестить .end() + 


