#include "FilesHierarchyPattern.h"
namespace  fs = std::filesystem;
using std::endl;
using std::cout;
using namespace std::chrono;

//fs::path filepath = fs::current_path(); - путь
//
//fs::create_directories("root"); - сделать папку
//fs::create_directories("root/branch1");
//
//fs::path a = "root/abc.txt"; - путь 
//fs::path b = "ex.txt";
//
//std::ofstream(a) << "test1"; - сделать файл 
//std::ofstream(b) << "tes2";


// Программа выводит время последней модификации файла
//  в виде количества секунд с 1 января 1970 года.
int main()
{
    std::wcout << "preferred_separator: " << fs::path::preferred_separator << std::endl;

    fs::path p = fs::current_path();

	//iBaseFileComponent* tree = new eFolderComposite("root");
	//tree->printLastModificateTime();
	//iBaseFileComponent* br1 = new eFolderComposite("root/", "br1");
	//br1->printLastModificateTime();
	//iBaseFileComponent* fil1 = new eFileLeaf("root/", "abc.tnt");
	//fil1->printLastModificateTime();

	eElement trr(new eFolderComposite("root1"));
	trr.printCreationDate();
	eElement tr2r(new eFolderComposite("root1/br1"));
	tr2r.printCreationDate();

	eElement fil2(new eFileLeaf("root1/", "abc.txt"));
	fil2.printCreationDate();

 //   std::string name = "root2";
 //   std::string slesh = "/";
 //   fs::path fold = name + slesh;

 //   std::string namefile = "abc";
 //   std::string extencial = ".txt";

 //   fs::path file1 = name + slesh + namefile + "1" + extencial;
 //   fs::path file2 = name + slesh + namefile + "2" + extencial;

	//fs::remove_all(fold);
	//fs::create_directories(fold);

	//std::ofstream(file1) << "1234567890";
	//std::ofstream(file2) << "-------";

 //   time_t s = to_time_t( fs::last_write_time(file1));
	//cout << std::ctime(&s) << endl;
	//cout << to_time_t(fs::last_write_time(file1)) << endl;

 //   cout << fs::file_size(fold) << endl;
 //   cout << fs::file_size(file1) << endl;
 //   cout << fs::file_size(file2) << endl;


    fs::path file_new = fs::current_path();

 //   std::ofstream(file1).put('1');
 //   s = to_time_t(fs::last_write_time(file1));
 //   cout << std::ctime(&s) << endl;
//    fs::path p = fs::current_path() / "sandbox";
//    fs::create_directories(p / "from");
//    std::ofstream(p / "from/file1.txt").put('a');
//    fs::create_directory(p / "to");
//
//    // fs :: rename (p / "from / file1.txt", p / "to /"); // ошибка: в каталог
//    fs::rename(p / "from/file1.txt", p / "to/file2.txt"); // OK
//// fs :: rename (p / "от", p / "до"); // ошибка: не пусто
//    fs::rename(p / "from", p / "to/subdir"); // OK
}

