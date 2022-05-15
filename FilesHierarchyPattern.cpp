#include "FilesHierarchyPattern.h"
int iBaseFileComponent::nestingLevel = 0;

iBaseFileComponent* eFolderComposite::createFolder(std::string _path, std::string _folderName)
{
	std::cout << "new folder";
	return new eFolderComposite(_path, _folderName);
}

iBaseFileComponent* eFolderComposite::createFile(std::string _path, std::string _fileName, unsigned int _size)
{
	std::cout << "new file";
	return new eFileLeaf(_path, _fileName);
}

