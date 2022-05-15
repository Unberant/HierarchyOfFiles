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

	virtual void addConnection(iBaseFileComponent* component) = 0;
	virtual void deleteConnection(iBaseFileComponent* component) = 0;

	virtual Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>* create_iterator()
	{
		return nullptr;
	}

	iBaseFileComponent* getParent() const
	{
		return parent;
	}
	void setParent(iBaseFileComponent* _parent)
	{
		this->parent = _parent;
	}

	virtual std::string getName() const = 0;

	std::string getFullPath() const
	{
		return filePath.u8string();
	}

	virtual iBaseFileComponent* getObject(fs::path _path) = 0;

	std::string getRootFolder() const
	{
		std::string strPath = filePath.string();
		size_t sleshIndex = strPath.find_first_of("/");
		if (sleshIndex == string::npos)
			return "no root folder";
		else
			return strPath.substr(0,sleshIndex);
	}

	virtual bool isFolder() const = 0;

	virtual void printDescription() = 0;

	std::string getCreationDate() const 
	{
		return  std::ctime(&fileCreationDate);
	}
	void printCreationDate() const
	{
		std::cout << filePath << " created on " << std::ctime(&fileCreationDate) << std::endl;
	}

	std::string getLastModificationDate() const   
	{
		return  std::ctime(&fileLastModificationDate);
	}
	void printLastModificationDate() const
	{
		std::cout << filePath << " modified on " << std::ctime(&fileLastModificationDate) << std::endl;
	}

	virtual unsigned int getSizeInBytes()	= 0;
	virtual unsigned int getNumberOfFiles()	= 0;

	virtual iBaseFileComponent* createFolder(std::string _path, std::string _folderName) = 0;
	virtual iBaseFileComponent* createFile(std::string _path, std::string _fileName, unsigned int _size) = 0;

	void modifyFileSize(const unsigned int& _newsize)
	{
		fs::resize_file(filePath, _newsize);
		setLastModificationDate(fs::last_write_time(filePath));
	}

	void removeFileFromSystem(iBaseFileComponent* component)
	{
		this->deleteConnection(component);
		fs::remove_all(component->filePath);
	}

	virtual void rename(const std::string& _newName) = 0;

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

	void setCreationDate(const std::filesystem::file_time_type& _creationDate)
	{
		fileCreationDate = to_time_t(_creationDate);
	}
	void setLastModificationDate(const std::filesystem::file_time_type& _modifDate)
	{
		this->fileLastModificationDate = to_time_t(_modifDate);
	}

	iBaseFileComponent* parent;
	fs::path filePath;
	time_t fileCreationDate;
	time_t fileLastModificationDate;
	static int nestingLevel;
};

class  eFolderComposite : public iBaseFileComponent
{
	using inherited = iBaseFileComponent*;
public:
	virtual ~eFolderComposite() = default;

	eFolderComposite(std::string _path, std::string _folderName): iBaseFileComponent(_path + _folderName)
	{
		fs::create_directories(_path + _folderName);
		setCreationDate(fs::last_write_time(filePath));
		this->fileLastModificationDate = this->fileCreationDate;
		folderName = _folderName;
	}
	eFolderComposite(std::string _folderName) : iBaseFileComponent(_folderName)
	{
		fs::create_directories(fs::current_path() / _folderName);
		setCreationDate(fs::last_write_time(filePath));
		this->fileLastModificationDate = this->fileCreationDate;

		size_t sleshIndex = _folderName.find_last_of("/");
		folderName = _folderName.substr(sleshIndex + 1, _folderName.size() - sleshIndex);
	}

	virtual Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>* create_iterator()
	{
		return new Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>(this->children);
	}

	void addConnection(iBaseFileComponent* _component) override
	{
		this->children.push_back(_component);
		_component->setParent(this);
	}
	void deleteConnection(iBaseFileComponent* _component) override
	{
		Iterator<iBaseFileComponent, std::list<inherited>>* it = create_iterator();
		for (it->first(); !it->isDone(); it->next()) 
		{
			//std::cout << _component->getName() << " =?= " << it->current()->getName() << std::endl;
			it->current()->deleteConnection(_component);
			if (_component->getName() == it->current()->getName())
			{
				children.remove(it->current());
				return;
			}
		}
	}

	std::string getName() const override
	{
		return folderName;
	}

	iBaseFileComponent* getObject(fs::path _path) override
	{
		return this;
	}

	bool isFolder() const override
	{
		return true;
	}

	virtual void printDescription() 
	{
		printIndentsByNestingLevel();
		++nestingLevel;
		std::cout << folderName << " [folder] " <<  filePath << std::endl;
		Iterator<iBaseFileComponent, std::list<inherited>>* it = create_iterator();
		for (it->first(); !it->isDone(); it->next()) 
		{
			it->current()->printDescription();
		}
		--nestingLevel;
	}

	unsigned int getSizeInBytes() override
	{
		unsigned int sizeOfAllFiles = 0;
		Iterator<iBaseFileComponent, std::list<inherited>>* it = create_iterator();
		for (it->first(); !it->isDone(); it->next()) 
		{
			sizeOfAllFiles += it->current()->getSizeInBytes();
		}
		return sizeOfAllFiles;
	}

	unsigned int getNumberOfFiles() override
	{
		unsigned int numberOfFiles = 0;
		Iterator<iBaseFileComponent, std::list<inherited>>* it = create_iterator();
		for (it->first(); !it->isDone(); it->next()) 
		{
			++numberOfFiles;
		}
		return numberOfFiles;
	}

	iBaseFileComponent* createFolder(std::string _path, std::string _folderName) override;
	iBaseFileComponent* createFile(std::string _path, std::string _fileName, unsigned int _size) override;

	void rename(const std::string& _newName) override
	{
		fs::path newPath;
		std::string strPath = filePath.string();
		size_t sleshIndex = strPath.find_last_of("/");

		if (sleshIndex == string::npos)
			newPath = _newName;
		else
		{
			strPath.erase(strPath.begin() + sleshIndex + 1, strPath.end());
			newPath = strPath + _newName;
		}

		fs::rename(filePath, newPath);

		this->filePath = newPath;
		this->folderName = _newName;
	}

private:
	std::list<inherited> children;
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
		setCreationDate(fs::last_write_time(filePath));
		this->fileLastModificationDate = this->fileCreationDate;
		file.fullname = _fileName;
	}

	void addConnection(iBaseFileComponent* _component) override
	{
		std::cout << "you can't link a " << file.fullname << " to " << _component->getName()
				<< " , call this method from a folder object" << std::endl;
	}

	void deleteConnection(iBaseFileComponent* _component) override
	{
		std::cout << "you can't unlink a " << file.fullname << " from " << _component->getName()
			<< " , call this method from a folder object" << std::endl;
	}

	std::string getName() const override
	{
		return file.fullname;
	}

	iBaseFileComponent* getObject(fs::path _path) override
	{
		return this;
	}

	bool isFolder() const override
	{
		return false;
	}

	void printDescription() override
	{
		printIndentsByNestingLevel();
		std::cout << file.getFullName() << " [file." << file.getExtensionWithoutDot() << "]" << std::endl;
	}

	unsigned int getSizeInBytes() override
	{
		return fs::file_size(filePath);
	}

	unsigned int getNumberOfFiles() override
	{
		return 0;
	}

	iBaseFileComponent* createFolder(std::string _path, std::string _folderName) override
	{
		std::cout << "you can not create folder in file";
		return nullptr;
	}
	iBaseFileComponent* createFile(std::string _path, std::string _fileName, unsigned int _size) override
	{
		std::cout << "you can not create file in file";
		return nullptr;
	}

	void rename(const std::string& _newName) override
	{
		fs::path newPath;
		std::string strPath = filePath.string();
		size_t sleshIndex = strPath.find_last_of("/");

		if (sleshIndex == string::npos)
			newPath = _newName;
		else
		{
			strPath.erase(strPath.begin() + sleshIndex + 1, strPath.end());
			newPath = strPath + _newName;
		}

		fs::rename(filePath, newPath);

		this->filePath = newPath;
		this->file.fullname = _newName;
	}
private:
	eFileData file;
};

void move(iBaseFileComponent* _file, eFolderComposite* _to)
{
}


class eElement {
public:
	~eElement() = default;

	eElement(iBaseFileComponent* newObject): baseElement(newObject) {}

	void addConnection(const eElement& that)
	{
		baseElement->addConnection(that.baseElement);
	}
	void deleteConnection(const eElement& that)
	{
		baseElement->deleteConnection(that.baseElement);
	}

	void printDescription()
	{
		baseElement->printDescription();
	}

	eElement getObject(fs::path _path)
	{
		return eElement(baseElement->getObject(_path));
	}

	std::string getRootFolder() const
	{
		return baseElement->getRootFolder();
	}

	bool isFolder() const
	{
		return baseElement->isFolder();
	}

	std::string getName() const
	{
		return baseElement->getName();
	}

	std::string getCreationDate() const
	{
		return baseElement->getCreationDate();
	}
	void printCreationDate() const
	{
		baseElement->printCreationDate();
	}

	std::string getLastModificationDate() const
	{
		return baseElement->getLastModificationDate();
	}
	void printLastModificationDate() const
	{
		baseElement->printLastModificationDate();
	}

	unsigned int getSizeInBytes()
	{
		return baseElement->getSizeInBytes();
	}

	unsigned int getNumberOfFiles()
	{
		return baseElement->getNumberOfFiles();
	}

	eElement createFolder(std::string _path, std::string _folderName)
	{
		eElement newFolder(baseElement->createFolder(_path, _folderName));
		this->addConnection(newFolder);
		return newFolder;
	}

	eElement creteFile(std::string _path, std::string _fileName, unsigned int _size)
	{
		eElement newFile(baseElement->createFile(_path,_fileName,_size));
		this->addConnection(newFile);
		return newFile;
	}

	void modifyFileSize(const unsigned int& _newsize)
	{
		baseElement->modifyFileSize(_newsize);
	}

	void removeFileFromSystem(const eElement& that)
	{
		baseElement->removeFileFromSystem(that.baseElement);
	}

	virtual void rename(const std::string& _newName) const
	{
		baseElement->rename(_newName);
	}

	void move(eElement* _file, eElement* _to)
	{
		
	}

private:
	iBaseFileComponent* baseElement;
};
