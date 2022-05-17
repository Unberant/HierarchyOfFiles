#include "FilesHierarchyPattern.h"

//===iBaseFileComponent:: public level
eFolderComposite::eFolderComposite(std::string _path, std::string _folderName) : iBaseFileComponent(_path + _folderName)
{
	if (isNotValid(_path + _folderName))
	{
		std::cout << "\nYou can not create file with such name: " << _path;
		return;;
	}
	fs::create_directories(_path + _folderName);
	setCreationDate(fs::last_write_time(filePath));
	this->fileLastModificationDate = this->fileCreationDate;
	folderName = _folderName;
}
eFolderComposite::eFolderComposite(std::string _fullPath) : iBaseFileComponent(_fullPath)
{
	if (isNotValid(_fullPath))
	{
		std::cout << "\nYou can not create file with such name: " << _fullPath;
		return;
	}
	fs::create_directories(fs::current_path() / _fullPath);
	setCreationDate(fs::last_write_time(filePath));
	this->fileLastModificationDate = this->fileCreationDate;

	size_t sleshIndex = _fullPath.find_last_of("/");
	folderName = _fullPath.substr(sleshIndex + 1, _fullPath.size() - sleshIndex);
}
eFolderComposite::eFolderComposite(std::string _fullPath, bool _createFolder) : iBaseFileComponent(_fullPath)
{
	if (isNotValid(_fullPath))
	{
		std::cout << "\nYou can not create file with such name: " << _fullPath;
		return;
	}
	if (_createFolder)
		fs::create_directories(fs::current_path() / _fullPath);
	setCreationDate(fs::last_write_time(filePath));
	this->fileLastModificationDate = this->fileCreationDate;

	size_t sleshIndex = _fullPath.find_last_of("/");
	folderName = _fullPath.substr(sleshIndex + 1, _fullPath.size() - sleshIndex);
}
eFolderComposite::eFolderComposite(ptrComponent _that, bool _createDirectory)
{
	if (_createDirectory)
		fs::create_directories(_that->getFullPath());

	this->fileCreationDate = _that->getCreationTime_t();
	this->fileLastModificationDate = _that->getLastModificationTime_t();
	this->folderName = _that->getName();
}

Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>* eFolderComposite::create_iterator() const
{
	return new Iterator<iBaseFileComponent, std::list<iBaseFileComponent*>>(this->children);
}

void eFolderComposite::addConnection(iBaseFileComponent* _component)
{
	this->children.push_back(_component);
	_component->setParent(this);
}
void eFolderComposite::deleteConnection(iBaseFileComponent* _component)
{
	Iterator<iBaseFileComponent, std::list<ptrComponent>>* it = create_iterator();
	for (it->first(); !it->isDone(); it->next())
	{
		//std::cout << _component->getName() << " =?= " << it->current()->getName() << std::endl;
		if (it->current()->isFolder())
			it->current()->deleteConnection(_component);
		if (_component->getName() == it->current()->getName())
		{
			children.remove(it->current());
			return;
		}
	}
}

std::string eFolderComposite::getName() const
{
	return folderName;
}
iBaseFileComponent* eFolderComposite::getObject(fs::path _path)
{
	return this;
}
unsigned int eFolderComposite::getSizeInBytes()
{
	unsigned int sizeOfAllFiles = 0;
	Iterator<iBaseFileComponent, std::list<ptrComponent>>* it = create_iterator();
	for (it->first(); !it->isDone(); it->next())
	{
		sizeOfAllFiles += it->current()->getSizeInBytes();
	}
	return sizeOfAllFiles;
}
unsigned int eFolderComposite::getNumberOfFiles()
{
	unsigned int numberOfFiles = 0;
	Iterator<iBaseFileComponent, std::list<ptrComponent>>* it = create_iterator();
	for (it->first(); !it->isDone(); it->next())
	{
		++numberOfFiles;
	}
	return numberOfFiles;
}

bool eFolderComposite::isFolder() const
{
	return true;
}

void eFolderComposite::printDescription() const
{
	printIndentsByNestingLevel();
	++nestingLevel;
	std::cout << this->folderName << " [folder] " << this->filePath << std::endl;
	Iterator<iBaseFileComponent, std::list<ptrComponent>>* it = create_iterator();
	for (it->first(); !it->isDone(); it->next())
	{
		it->current()->printDescription();
	}
	--nestingLevel;
}

void eFolderComposite::rename(const std::string& _newName)
{
	fs::path newPath;
	std::string strPath = filePath.string();
	size_t sleshIndex = strPath.find_last_of("/");

	if (sleshIndex == std::string::npos)
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

iBaseFileComponent* eFolderComposite::copyHere(iBaseFileComponent* _objectToCopy)
{
	this->createFilesInSystem(_objectToCopy);
	iBaseFileComponent* copiedFiles;

	std::string newPath = this->getFullPath() + "/";

	if (_objectToCopy->isFolder())
		copiedFiles = getCopiedFiles(newPath + _objectToCopy->getName() + COPY_LABEL);
	else
	{
		newPath += _objectToCopy->getNameWithOutExtension() + COPY_LABEL + "." + _objectToCopy->getExtension();
		copiedFiles = new eFileLeaf(newPath, false);
	}

	this->addConnection(copiedFiles);

	return copiedFiles;
}
iBaseFileComponent* eFolderComposite::moveHere(iBaseFileComponent* _objectToMove)
{
	iBaseFileComponent* movedFiles = this->copyHere(_objectToMove);

	_objectToMove->getParent()->removeFileFromSystem(_objectToMove);

	return movedFiles;
}

void eFolderComposite::createFilesInSystem(const ptrComponent _componentToCopy) const 
{
	const auto copyOptions = fs::copy_options::overwrite_existing | fs::copy_options::recursive;
	std::string modifiedName = _componentToCopy->getName();

	if (_componentToCopy->isFolder())
		modifiedName += COPY_LABEL;
	else
	{
		size_t dotIndex = modifiedName.find_last_of(".");
		modifiedName.insert(dotIndex, COPY_LABEL);
	}
	std::string newPath = this->getFullPath() + "/" + modifiedName;

	fs::copy(_componentToCopy->getFullPath(), newPath, copyOptions);
}

iBaseFileComponent* eFolderComposite::findIn(std::string name_mask)
{
	std::cout << "folder name:" << this->getName() << ", name mask:" << name_mask << std::endl;

	Iterator<iBaseFileComponent, std::list<ptrComponent>>* it = create_iterator();
	for (it->first(); !it->isDone(); it->next())
	{
		if (it->current()->getName() == name_mask)
		{
			std::cout << "folder name:" << it->current()->getName() << ", name mask:" << name_mask << std::endl;
			return  it->current()->findIn(name_mask);
		}
		else
			return nullptr;
	}
}

iBaseFileComponent* eFolderComposite::createFolder(std::string _path, std::string _folderName)
{
	std::cout << "new folder\n";
	return new eFolderComposite(_path, _folderName);
}
iBaseFileComponent* eFolderComposite::createFolder(std::string _fullPath)
{
	std::cout << "new folder\n";
	return new eFolderComposite(_fullPath);
}

iBaseFileComponent* eFolderComposite::createFile(std::string _path, std::string _fileName, unsigned int _size)
{
	std::cout << "new file\n";
	return new eFileLeaf(_path, _fileName, _size);
}
iBaseFileComponent* eFolderComposite::createFile(std::string _fullPath)
{
	std::cout << "new file\n";

	size_t lastSleshIndex = _fullPath.find_last_of("/");

	std::string strPath = _fullPath.substr(0, lastSleshIndex + 1);
	std::string strName = _fullPath.substr(lastSleshIndex + 1, _fullPath.size() - lastSleshIndex);

	return new eFileLeaf(strPath, strName);
}

//===iBaseFileComponent:: private level
std::string eFolderComposite::getNameWithOutExtension() const
{
	return this->folderName;
}
std::string eFolderComposite::getExtension() const
{
	return "";
}