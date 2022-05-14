#pragma once
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <list>
#include <string>
#include <iterator>
namespace fs = std::filesystem;
using namespace std::chrono_literals;
using string = std::string;

template <typename TP>
std::time_t to_time_t(TP tp)
{
	using namespace std::chrono;
	auto sctp = time_point_cast<system_clock::duration>
										(tp - TP::clock::now() + system_clock::now());
	return system_clock::to_time_t(sctp);
}

struct eFileData
{
	eFileData() = default;
	eFileData(std::string _name, std::string _extensionWithoutDot)
	{
		fullname = _name + "." + _extensionWithoutDot;
	}
	eFileData(string _fullname)
	{
		fullname = _fullname;
	}

	string getExtensionWithoutDot()
	{
		size_t dotIndex = fullname.find_last_of(".");
		return fullname.substr(dotIndex + 1,fullname.size()-dotIndex);

	}
	string getFullName()
	{
		return fullname;
	}

	std::string fullname;
};

template<typename T, typename U>
class Iterator {
public:
	//using filesIt = std::list<T*>::iterator;
	typedef typename std::list<T*>::iterator filesIt;

	Iterator(U& const _children) : children(_children)
	{
		it = children.begin();
	}
	void first()
	{
		it = children.begin();
	}
	void next()
	{
		it++;
	}
	T* current()
	{
		return *it;
	}
	bool isDone()
	{
		return (it == children.end());
	}

private:
	U& children;
	filesIt it;
};

class iBaseFileComponent 
{
public:
	virtual ~iBaseFileComponent() = default;

	iBaseFileComponent(std::string _path)
	{
		filePath = _path;
	}
	iBaseFileComponent(fs::path _path)
	{
		filePath = _path;
	}

	virtual void addToFolder(iBaseFileComponent* component)			{};
	virtual void removeFromFolder(iBaseFileComponent* component)	{};

	virtual Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>* create_iterator()
	{
		return nullptr;
	}

	void setParent(iBaseFileComponent* _parent)
	{
		this->parent = _parent;
	}
	iBaseFileComponent* getParent() const
	{
		return parent;
	}

	virtual void setName(std::string) = 0;
	virtual std::string getName() = 0;

	std::string getFullPath()
	{
		return filePath.u8string();
	}

	virtual iBaseFileComponent* getObject(fs::path _path) = 0;

	std::string getRootFolder()
	{
		std::string strPath = filePath.string();
		size_t sleshIndex = strPath.find_first_of("/");
		if (sleshIndex == string::npos)
			return "no root folder";
		else
			return strPath.substr(0,sleshIndex);
	}

	virtual bool isFolder() = 0;

	virtual void printDescription() = 0;

	std::string getCreationDate()
	{
		time_t s = to_time_t(fs::last_write_time(filePath));
		return  std::ctime(&s);
	}
	void printCreationDate()
	{
		time_t s = to_time_t(fs::last_write_time(filePath));
		std::cout << std::ctime(&s) << std::endl;
	}

	//  to work
	std::string getLastModificationDate()  
	{
		time_t s = to_time_t(fs::last_write_time(filePath));
		return  std::ctime(&s);
	}

	virtual unsigned int getSizeInBytes() = 0;
	virtual unsigned int getNumberOfFiles() = 0;

	//void createFolder(std::string _path, std::string _name)
	//{
	//	iBaseFileComponent* newFolder = new("root");
	//}

	//void createFile(std::string _path, std::string _name, unsigned int _size)
	//{
	//	std::cout << "new file";
	//	iBaseFileComponent* fil1 = new eFileLeaf("root/", "abc.tnt");

	//}

	//virtual iBaseFileComponent* clone() {
	//	return nullptr;
	//}
	//iBaseFileComponent* move() {
	//	iBaseFileComponent* buffer = this->clone();
	//	this->getParent()->removeFromFolder(this);
	//	return buffer;
	//}

protected:
	virtual void printIndentsByNestingLevel()
	{
		if (nestingLevel <= 0) nestingLevel = 0;
		std::cout << std::string(nestingLevel,'-');
	}

	iBaseFileComponent* parent;
	fs::path filePath;
	time_t fileCreaitonDate;
	time_t fileLastModificationDate;
	static int nestingLevel;
};

class  eFolderComposite : public iBaseFileComponent
{
	using inhereted = iBaseFileComponent*;
public:
	virtual ~eFolderComposite() = default;

	eFolderComposite(std::string _path, std::string _folderName): iBaseFileComponent(_path + _folderName)
	{
		fs::create_directories(_path + _folderName);
		fileCreaitonDate = to_time_t(fs::last_write_time(filePath));
		folderName = _folderName;
	}
	eFolderComposite(std::string _folderName) : iBaseFileComponent(_folderName)
	{
		fs::create_directories(fs::current_path() / _folderName);
		fileCreaitonDate = to_time_t(fs::last_write_time(filePath));

		size_t sleshIndex = _folderName.find_last_of("/");
		folderName = _folderName.substr(sleshIndex + 1, _folderName.size() - sleshIndex);
	}

	virtual Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>* create_iterator()
	{
		return new Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>(this->children);
	}

	void addToFolder(iBaseFileComponent* _component) override
	{
		this->children.push_back(_component);
		_component->setParent(this);
	}
	void removeFromFolder(iBaseFileComponent* _component) override
	{
		children.remove(_component);
		_component->setParent(nullptr);
	}

	void setName(std::string _folderName) override
	{
		folderName = _folderName;
	}
	std::string getName() override
	{
		return folderName;
	}

	iBaseFileComponent* getObject(fs::path _path) override
	{
		return this;
	}

	bool isFolder() override
	{
		return true;
	}

	virtual void printDescription()
	{
		printIndentsByNestingLevel();
		++nestingLevel;
		std::cout << folderName << " [folder] " <<  filePath << std::endl;
		Iterator<iBaseFileComponent, std::list<inhereted>>* it = create_iterator();
		for (it->first(); !it->isDone(); it->next()) {
			it->current()->printDescription();
		}
		--nestingLevel;
	}

	unsigned int getSizeInBytes() override
	{
		unsigned int sizeOfAllFiles = 0;
		Iterator<iBaseFileComponent, std::list<inhereted>>* it = create_iterator();
		for (it->first(); !it->isDone(); it->next()) {
			sizeOfAllFiles += it->current()->getSizeInBytes();
		}
		return sizeOfAllFiles;
	}

	unsigned int getNumberOfFiles() override
	{
		unsigned int numberOfFiles = 0;
		Iterator<iBaseFileComponent, std::list<inhereted>>* it = create_iterator();
		for (it->first(); !it->isDone(); it->next()) {
			++numberOfFiles;
		}
		return numberOfFiles;
	}

	//void createFolder(std::string _path, std::string _folderName) 
	//{
	//	std::cout << "new folder";
	//	//fs::create_directories(_path + _folderName);
	//	//fileCreaitonDate = to_time_t(fs::last_write_time(filePath));
	//	//folderName = _folderName;
	//}

private:
	std::list<inhereted> children;
	string folderName;
};

class eFileLeaf : public  iBaseFileComponent
{
	using inhereted = iBaseFileComponent*;
public:
	virtual ~eFileLeaf() = default;

	eFileLeaf(std::string _path, std::string _fileName) : iBaseFileComponent(_path + _fileName)
	{
		std::ofstream(_path + _fileName);
		fileCreaitonDate = to_time_t(fs::last_write_time(filePath));
		file.fullname = _fileName;
	}

	void setName(std::string _folderName) override
	{
		file.fullname = _folderName;
	}
	std::string getName() override
	{
		return file.fullname;
	}

	iBaseFileComponent* getObject(fs::path _path) override
	{
		return this;
	}

	bool isFolder() override
	{
		return false;
	}

	void printDescription() override
	{
		printIndentsByNestingLevel();
		std::cout << file.getFullName() << " [file." << file.getExtensionWithoutDot() << "]" << std::endl;
	}

	unsigned int getSizeInBytes()
	{
		return fs::file_size(filePath);
	}

	unsigned int getNumberOfFiles() override
	{
		return 0;
	}

	//void createFile(std::string _path, std::string _name, unsigned int _size)
	//{
	//	std::cout << "new file";

	//}
private:
	eFileData file;
};

class eElement {
public:
	~eElement() = default;

	eElement(iBaseFileComponent* newObject): baseElement(newObject) {}

	void addToFolder(const eElement& that)
	{
		baseElement->addToFolder(that.baseElement);
	}
	void removeFromFolder(const eElement& that)
	{
		baseElement->removeFromFolder(that.baseElement);
	}

	void printDescription()
	{
		baseElement->printDescription();
	}

	eElement getObject(fs::path _path)
	{
		return eElement(baseElement->getObject(_path));
	}

	std::string getRootFolder()
	{
		return baseElement->getRootFolder();
	}

	bool isFolder()
	{
		return baseElement->isFolder();
	}

	std::string getName()
	{
		return baseElement->getName();
	}

	std::string getCreationDate()
	{
		return baseElement->getCreationDate();
	}
	void printCreationDate()
	{
		baseElement->printCreationDate();
	}

	std::string getLastModificationDate()
	{
		return baseElement->getLastModificationDate();
	}

	unsigned int getSizeInBytes() 
	{
		return baseElement->getSizeInBytes();
	}

	unsigned int getNumberOfFiles()
	{
		return baseElement->getNumberOfFiles();
	}

	//static iBaseFileComponent* createFolder(std::string _path, std::string _name)
	//{
	//	//baseElement->createFolder(_path, _name);
	//	return new eFolderComposite(_path, _name);
	//}
	//static iBaseFileComponent* createFile(std::string _path, std::string _name, unsigned int _size)
	//{
	//	//baseElement->createFile(_path, _name, _size);
	//	return new eFileLeaf(_path, _name);
	//}

	//void createFolder(std::string _path, std::string _name)
	//{
	//	iBaseFileComponent* newFolder = new eFolderComposite(_path,_name);
	//}

	//void createFile(std::string _path, std::string _name, unsigned int _size) 
	//{
	//	std::cout << "new file";

	//}
private:
	iBaseFileComponent* baseElement;
};
