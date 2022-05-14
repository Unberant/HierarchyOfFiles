#pragma once
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
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
	std::string fullname;
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
	void printCreationDate()
	{
		time_t s = to_time_t(fs::last_write_time(filePath));
		std::cout << std::ctime(&s) << std::endl;
	}
	virtual void add(iBaseFileComponent* component)		{};
	virtual void remove(iBaseFileComponent* component)	{};

	void setParent(iBaseFileComponent* _parent)
	{
		this->parent = _parent;
	}
	iBaseFileComponent* getParent() const
	{
		return parent;
	}


	virtual iBaseFileComponent* clone() {
		return nullptr;
	}
	iBaseFileComponent* move() {
		iBaseFileComponent* buffer = this->clone();
		this->getParent()->remove(this);
		return buffer;
	}

protected:
	iBaseFileComponent* parent;
	fs::path filePath;
	time_t fileCreaitonDate;
	time_t fileLastModificationDate;
};

class  eFolderComposite : public iBaseFileComponent
{
	using inhereted = iBaseFileComponent*;
public:
	virtual ~eFolderComposite() = default;

	eFolderComposite(std::string _path, std::string _FolderName): iBaseFileComponent(_path + _FolderName)
	{
		fs::create_directories(_path + _FolderName);
		fileCreaitonDate = to_time_t(fs::last_write_time(filePath));
		folderName = _FolderName;
	}
	eFolderComposite(std::string _FolderName) : iBaseFileComponent(fs::current_path() / _FolderName)
	{
		fs::create_directories(fs::current_path() / _FolderName);
		fileCreaitonDate = to_time_t(fs::last_write_time(filePath));
		folderName = _FolderName;
	}

private:
	std::list<inhereted> children_;
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
private:
	eFileData file;
};

class eElement {
public:
	~eElement() = default;

	eElement(iBaseFileComponent* newObject): baseElement(newObject) {}

	void printCreationDate()
	{
		baseElement->printCreationDate();
	}
private:
	iBaseFileComponent* baseElement;
};
