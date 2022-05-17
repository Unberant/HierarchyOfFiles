#pragma once
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <fstream>
#include <list>
#include <string>
#include <functional>

const std::string COPY_LABEL = "-Copy";

typedef unsigned int unInt;
typedef std::filesystem::file_time_type timeInFile;

enum invalidCharacters
{
	SLASH	= '\\',
	SEMI_COLON	= ';',
	COLON	= ':',
	ASTERISK	= '*',
	QUESTION_MARK	= '?',
	LESS_THAN	= '<',
	GREATER_THAN	= '>',
	PIPE	= '|',
	ACUTE	= '`',
	QUOTE	= '"',
	APOSTROPHE	= '\''
};

namespace fs = std::filesystem;
using namespace std::chrono_literals;


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
	eFileData(std::string _fullname)
	{
		fullname = _fullname;
	}

	std::string getExtensionWithoutDot() const
	{
		size_t dotIndex = fullname.find_last_of(".");
		return fullname.substr(dotIndex + 1,fullname.size()-dotIndex);
	}
	std::string getFullName() const
	{
		return fullname;
	}

	std::string fullname;
};

template<typename T, typename U>
class Iterator {
public:
	typedef typename std::list<T*>::iterator filesIt;

	Iterator(const U &_children) : children(_children)
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
	T* getPrevious()
	{
		return *(it - 1);
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
	U children;
	filesIt it;
};

class iBaseFileComponent 
{
	typedef iBaseFileComponent* ptrComponent;
public:
	virtual ~iBaseFileComponent() = default;

	iBaseFileComponent() = default;
	iBaseFileComponent(std::string _path);
	iBaseFileComponent(fs::path _path);

	iBaseFileComponent(iBaseFileComponent& other) = default;
	iBaseFileComponent(iBaseFileComponent&& other) = default;

	virtual void addConnection (ptrComponent component) = 0;
	virtual void deleteConnection	(ptrComponent component) = 0;

	virtual Iterator<iBaseFileComponent, std::list<ptrComponent>>* create_iterator() const { return nullptr; }

	ptrComponent	getCopiedFiles	( std::string _path );

	ptrComponent	getParent ()	const;
	void	setParent (ptrComponent _parent);

	virtual	std::string getName() const	= 0;
	virtual	std::string getNameWithOutExtension() const	= 0;
	virtual	std::string getExtension() const	= 0;

	std::string getFullPath() const;

	virtual ptrComponent getObject (fs::path _path) = 0;

	std::string getRootFolder() const;

	std::string getCreationDate()	const;
	time_t getCreationTime_t()	const;
	void printCreationDate()	const;

	std::string getLastModificationDate()	const;
	time_t getLastModificationTime_t()	const;
	void printLastModificationDate()	const;

	virtual unInt getSizeInBytes()	= 0;
	virtual unInt getNumberOfFiles()	= 0;

	virtual bool isFolder()	const	= 0;

	virtual void printDescription()	const	= 0;

	void modifyFileSize (const unInt& _newsize);

	void removeFileFromSystem (ptrComponent component);

	virtual void rename(const std::string& _newName)	= 0;

	virtual ptrComponent copyHere (ptrComponent _objectToCopy)	= 0;
	virtual ptrComponent moveHere (ptrComponent _objectToMove)	= 0;

	virtual ptrComponent findIn (std::string name_mask)	= 0;
	
	virtual ptrComponent createFolder (std::string _path, std::string _folderName)	= 0;
	virtual ptrComponent createFolder (std::string _fullPath)	= 0;
	virtual ptrComponent createFile (std::string _path, std::string _fileName, unInt _size)	= 0;
	virtual ptrComponent createFile (std::string _path)	= 0;

	static std::vector<ptrComponent> findIn(ptrComponent _folder, std::string name_mask);

protected:
	
	void printIndentsByNestingLevel()	const;

	void setCreationDate (const timeInFile& _creationDate);
	void setLastModificationDate (const timeInFile& _modifDate);

	virtual void createFilesInSystem (const ptrComponent _componentToCopy)	const = 0;

	bool isNotValid (const std::string& _path)	const;

	ptrComponent parent;
	fs::path filePath;
	time_t fileCreationDate;
	time_t fileLastModificationDate;
	static int nestingLevel;
};

class  eFolderComposite : public iBaseFileComponent
{
	typedef iBaseFileComponent* ptrComponent;
public:
	virtual ~eFolderComposite() = default;

	eFolderComposite (std::string _path, std::string _folderName);
	eFolderComposite (std::string _fullPath);
	eFolderComposite (std::string _fullPath, bool _createFolder);
	eFolderComposite (ptrComponent _that, bool _createDirectory);

	Iterator<iBaseFileComponent, std::list<ptrComponent>>* create_iterator() const override;

	void	addConnection	(ptrComponent _component)	override;
	void	deleteConnection(ptrComponent _component)	override;

	std::string	 getName()	const	override;

	ptrComponent getObject(fs::path _path)	override;

	unInt getSizeInBytes()	override;

	unInt getNumberOfFiles()	override;

	bool isFolder()	const	override;

	virtual void printDescription()	const;

	void rename	(const std::string& _newName)	override;

	ptrComponent	copyHere(ptrComponent _objectToCopy)	override;
	ptrComponent	moveHere(ptrComponent _objectToMove)	override;

	ptrComponent	findIn	(std::string name_mask)	override;

	ptrComponent	createFolder(std::string _path, std::string _folderName)	override;
	ptrComponent	createFolder(std::string _fullPath)	override;
	ptrComponent	createFile(std::string _path, std::string _fileName, unInt _size) override;
	ptrComponent	createFile(std::string _fullPath) override;

private:
	void	createFilesInSystem(const ptrComponent _componentToCopy) const override;

	std::string	getNameWithOutExtension()	const override;
	std::string getExtension()	const override;

	std::list<ptrComponent> children;
	std::string	folderName;
};


class eFileLeaf : public  iBaseFileComponent
{
	typedef iBaseFileComponent* ptrComponent;
public:
	virtual ~eFileLeaf() = default;

	eFileLeaf (std::string _path,	std::string _fileName);
	eFileLeaf (std::string _path,	std::string _fileName, unInt _size);
	eFileLeaf (std::string _fullPath, bool _createFile);
	eFileLeaf(ptrComponent _that, bool _createDirectory);

	void addConnection(ptrComponent _component)	override;
	void deleteConnection(ptrComponent _component)	override;

	std::string getName()	const	override;

	ptrComponent getObject(fs::path _path)	override;

	unInt getSizeInBytes()	override;

	unInt getNumberOfFiles()	override;

	bool isFolder()	const override;

	void printDescription()	const override;


	void rename(const std::string& _newName)	override;

	ptrComponent	copyHere(ptrComponent _objectToCopy)	override;

	ptrComponent	moveHere(ptrComponent _objectToMove)	override;

	ptrComponent	findIn(std::string name_mask)	override;

	ptrComponent	createFolder(std::string _path, std::string _folderName)	override;
	ptrComponent	createFolder(std::string _fullPath)	override;
	ptrComponent	createFile(std::string _path, std::string _fileName, unInt _size) override;
	ptrComponent	createFile(std::string _fullPath)	override;

private:

	void createFilesInSystem(const ptrComponent _componentToCopy)	const override {}

	std::string getNameWithOutExtension()	const override;
	std::string getExtension()	const override;

	eFileData file;
};


class eElement {
public:
	~eElement() = default;

	eElement(iBaseFileComponent* newObject):	baseElement(newObject) { }

	void addConnection(const eElement& that)	{baseElement->addConnection(that.baseElement); }
	void deleteConnection(const eElement& that)	{baseElement->deleteConnection(that.baseElement); }

	void printDescription() {baseElement->printDescription(); }

	std::string getFullPath()	const	{return baseElement->getFullPath(); }

	eElement getObject(fs::path _path)	{return eElement(baseElement->getObject(_path)); }

	std::string getRootFolder()	const	{return baseElement->getRootFolder(); }

	bool isFolder()	const	{return baseElement->isFolder(); }

	std::string getName()	const	{return baseElement->getName(); }

	std::string getCreationDate()	const	{return baseElement->getCreationDate(); }
	void printCreationDate()	const	{baseElement->printCreationDate(); }

	std::string getLastModificationDate()	const	{return baseElement->getLastModificationDate(); }
	void printLastModificationDate() const	{baseElement->printLastModificationDate(); }

	unInt getSizeInBytes()	{return baseElement->getSizeInBytes(); }

	unInt getNumberOfFiles()	{return baseElement->getNumberOfFiles(); }

	eElement createFolder(std::string _path, std::string _folderName)
	{
		eElement newFolder(baseElement->createFolder(_path, _folderName));
		this->addConnection(newFolder);
		return newFolder;
	}
	eElement creteFile(std::string _path, std::string _fileName, unInt _size)
	{
		eElement newFile(baseElement->createFile(_path,_fileName,_size));
		this->addConnection(newFile);
		return newFile;
	}
	eElement creteFile(std::string _fullPath)
	{
		eElement newFile(baseElement->createFile(_fullPath));
		this->addConnection(newFile);
		return newFile;
	}

	void modifyFileSize(const unInt& _newsize)	{baseElement->modifyFileSize(_newsize); }

	void removeFileFromSystem(const eElement& that)	{baseElement->removeFileFromSystem(that.baseElement); }

	virtual void rename(const std::string& _newName)	const	{baseElement->rename(_newName); }

	eElement copyHere(eElement& _objectToCopy)	{return eElement(baseElement->copyHere(_objectToCopy.baseElement)); }
	eElement moveHere(eElement& _objectToMove)	{return eElement(baseElement->moveHere(_objectToMove.baseElement)); }

	static std::vector<eElement> findIn(const eElement &_folder, const std::string& name_mask)
	{
		std::vector<eElement> components;
		for (auto const component : iBaseFileComponent::findIn(_folder.baseElement, name_mask))
		{
			components.push_back(component);
		}
		return components;
	}
	static void printCollection(const std::vector<eElement> &_collection)
	{
		for (auto const &component : _collection)
		{
			std::cout << component.baseElement->getName() << std::endl;
		}
	}

private:
	iBaseFileComponent* baseElement;
};