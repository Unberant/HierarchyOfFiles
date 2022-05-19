#include "FilesHierarchyPattern.h"

//===iBaseFileComponent:: public level
eFileLeaf::eFileLeaf(std::string _path, std::string _fileName) : iBaseFileComponent(_path + _fileName)
{
	if (isNotValid(_path + _fileName))
	{
		std::cout << "\nYou can not create file with such name: " << _path;
		exit;
	}
	std::ofstream(_path + _fileName);
	setCreationDate(fs::last_write_time(filePath));
	this->fileLastModificationDate = this->fileCreationDate;
	file.fullname = _fileName;
}
eFileLeaf::eFileLeaf(std::string _path, std::string _fileName, unInt _size) : iBaseFileComponent(_path + _fileName)
{
	if (isNotValid(_path + _fileName))
	{
		std::cout << "\nYou can not create file with such name: " << _path;
		return;
	}
	std::ofstream(_path + _fileName);
	this->modifyFileSize(_size);
	setCreationDate(fs::last_write_time(filePath));
	this->fileLastModificationDate = this->fileCreationDate;
	file.fullname = _fileName;
}
eFileLeaf::eFileLeaf(std::string _fullPath, bool _createFile)
{
	if (isNotValid(_fullPath))
	{
		std::cout << "\nYou can not create file with such name: " << _fullPath;
		return;
	}
	size_t lastSleshIndex = _fullPath.find_last_of("/");

	std::string strPath = _fullPath.substr(0, lastSleshIndex + 1);
	std::string strName = _fullPath.substr(lastSleshIndex + 1, _fullPath.size() - lastSleshIndex);

	this->filePath = (strPath + strName);

	if (_createFile)
		std::ofstream(strPath + strName);
	
	setCreationDate(fs::last_write_time(filePath));
	this->fileLastModificationDate = this->fileCreationDate;
	file.fullname = strName;
}
eFileLeaf::eFileLeaf(ptrComponent _that, bool _createDirectory)
{
	if (_createDirectory)
		std::ofstream(_that->getFullPath());

	this->fileCreationDate = _that->getCreationTime_t();
	this->fileLastModificationDate = _that->getLastModificationTime_t();
	this->file.fullname = _that->getName();
}
void eFileLeaf::addConnection(iBaseFileComponent* _component) 
{
	std::cout << "you can't link a " << file.fullname << " to " << _component->getName()
		<< " , call this method from a folder object" << std::endl;
}
void eFileLeaf::deleteConnection(iBaseFileComponent* _component)
{
	std::cout << "you can't unlink a " << file.fullname << " from " << _component->getName()
		<< " , call this method from a folder object" << std::endl;
}

std::string eFileLeaf::getName() const
{
	return file.fullname;
}

iBaseFileComponent* eFileLeaf::getObject(fs::path _path)
{
	return this;
}

unsigned int eFileLeaf::getSizeInBytes()
{
	return fs::file_size(filePath);
}

unsigned int eFileLeaf::getNumberOfFiles()
{
	return 0;
}

bool eFileLeaf::isFolder() const
{
	return false;
}

void eFileLeaf::printDescription() const
{
	printIndentsByNestingLevel();
	std::cout << file.getFullName() << " [file." << file.getExtensionWithoutDot() << "]" << std::endl;
}


void eFileLeaf::rename(const std::string& _newNameWithOutExtension)
{
	fs::path newPath;
	std::string strPath = filePath.string();
	size_t sleshIndex = strPath.find_last_of("/");

	if (sleshIndex == std::string::npos)
		newPath = _newNameWithOutExtension + "." + file.getExtensionWithoutDot();
	else
	{
		strPath.erase(strPath.begin() + sleshIndex + 1, strPath.end());
		newPath = strPath + _newNameWithOutExtension + "." + file.getExtensionWithoutDot();
	}

	fs::rename(filePath, newPath);

	this->filePath = newPath;
	this->file.fullname = _newNameWithOutExtension + "." + file.getExtensionWithoutDot();
}

iBaseFileComponent* eFileLeaf::copyHere(iBaseFileComponent* _objectToCopy)
{
	std::cout << "you can't copy a " << _objectToCopy->getName() << " to " << file.fullname
		<< "[file], call this method from a folder object" << std::endl;
	return nullptr;
}

iBaseFileComponent* eFileLeaf::moveHere(iBaseFileComponent* _objectToMove)
{
	std::cout << "you can't move a " << _objectToMove->getName() << " to " << file.fullname
		<< "[file], call this method from a folder object" << std::endl;
	return nullptr;
}

std::vector<iBaseFileComponent*> eFileLeaf::findIn(std::string name_mask) 
{
	std::vector<ptrComponent> found_component;
	if (isPatternIn(this->getName(),name_mask))
		found_component.push_back(this);
	
	return found_component;
}

iBaseFileComponent* eFileLeaf::createFolder(std::string _path, std::string _folderName)
{
	std::cout << "you can not create folder in file";
	return nullptr;
}
iBaseFileComponent* eFileLeaf::createFolder(std::string _fullPath)
{
	std::cout << "you can not create folder in file";
	return nullptr;
}
iBaseFileComponent* eFileLeaf::createFile(std::string _path, std::string _fileName, unsigned int _size)
{
	std::cout << "you can not create file in file";
	return nullptr;
}
iBaseFileComponent* eFileLeaf::createFile(std::string _fullPath)
{
	std::cout << "you can not create file in file";
	return nullptr;
}

//===iBaseFileComponent:: private level
std::string eFileLeaf::getNameWithOutExtension() const
{
	size_t dotIndex = file.fullname.find_last_of(".");
	return file.fullname.substr(0, dotIndex);
}

std::string eFileLeaf::getExtension() const
{
	return file.getExtensionWithoutDot();
}
