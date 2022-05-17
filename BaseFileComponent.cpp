#include "FilesHierarchyPattern.h"
//===iBaseFileComponent:: static
int iBaseFileComponent::nestingLevel = 0;

static std::vector<iBaseFileComponent*> findIn(iBaseFileComponent* _folder, std::string name_mask)
{
	std::vector<iBaseFileComponent*> allDetections;

	_folder->findIn(name_mask);

}


//===iBaseFileComponent:: public level
iBaseFileComponent::iBaseFileComponent(std::string _path)
{
	filePath = _path;
}
iBaseFileComponent::iBaseFileComponent(fs::path _path)
{
	filePath = _path;
}

iBaseFileComponent* iBaseFileComponent::getParent() const
{
	return parent;
}
void iBaseFileComponent::setParent(iBaseFileComponent* _parent)
{
	this->parent = _parent;
}

std::string iBaseFileComponent::getFullPath() const
{
	return filePath.u8string();
}

std::string iBaseFileComponent::getCreationDate() const
{
	return  std::ctime(&fileCreationDate);
}
time_t iBaseFileComponent::getCreationTime_t() const
{
	return fileCreationDate;
}
void iBaseFileComponent::printCreationDate() const
{
	std::cout << filePath << " created on " << std::ctime(&fileCreationDate) << std::endl;
}

std::string iBaseFileComponent::getLastModificationDate() const
{
	return  std::ctime(&fileLastModificationDate);
}
time_t iBaseFileComponent::getLastModificationTime_t() const
{
	return fileLastModificationDate;
}

void iBaseFileComponent::printLastModificationDate() const
{
	std::cout << filePath << " modified on " << std::ctime(&fileLastModificationDate) << std::endl;
}
std::string iBaseFileComponent::getRootFolder() const
{
	std::string strPath = filePath.string();
	size_t sleshIndex = strPath.find_first_of("/");
	if (sleshIndex == std::string::npos)
		return "";
	else
		return strPath.substr(0, sleshIndex + 1);
}

iBaseFileComponent* iBaseFileComponent::getCopiedFiles(std::string _path)
{
	iBaseFileComponent* rootBufferComponent = new eFolderComposite(_path, false);
	for (const auto& entry : fs::directory_iterator(_path))
	{
		if (fs::is_directory(entry.path()))
		{
			std::string newPath = entry.path().parent_path().u8string() + "/" + entry.path().filename().u8string();
			rootBufferComponent->addConnection(getCopiedFiles(newPath));
		}
		else
		{
			std::string newPath = entry.path().parent_path().u8string() + "/" + entry.path().filename().u8string();
			rootBufferComponent->addConnection(new eFileLeaf(newPath, false));
		}
	}
	return rootBufferComponent;
}

void iBaseFileComponent::modifyFileSize(const unsigned int& _newsize)
{
	fs::resize_file(filePath, _newsize);
	setLastModificationDate(fs::last_write_time(filePath));
}

void iBaseFileComponent::removeFileFromSystem(iBaseFileComponent* component)
{
	this->deleteConnection(component);
	fs::remove_all(component->filePath);
}

//===iBaseFileComponent:: private level
void iBaseFileComponent::printIndentsByNestingLevel() const
{
	if (nestingLevel <= 0) nestingLevel = 0;
	std::cout << std::string(nestingLevel, '-');
}

void iBaseFileComponent::setCreationDate(const std::filesystem::file_time_type& _creationDate)
{
	fileCreationDate = to_time_t(_creationDate);
}

void iBaseFileComponent::setLastModificationDate(const std::filesystem::file_time_type& _modifDate)
{
	this->fileLastModificationDate = to_time_t(_modifDate);
}

bool iBaseFileComponent::isNotValid(const std::string &_path) const
{
	for (auto const& character : _path)
	{
		switch (character)
		{
		case invalidCharacters::QUESTION_MARK: return true;
		case invalidCharacters::GREATER_THAN: return true;
		case invalidCharacters::APOSTROPHE: return true;
		case invalidCharacters::SEMI_COLON: return true;
		case invalidCharacters::LESS_THAN: return true;
		case invalidCharacters::ASTERISK: return true;
		case invalidCharacters::ACUTE:	return true;
		case invalidCharacters::QUOTE:	return true;
		case invalidCharacters::COLON: return true;
		case invalidCharacters::SLASH: return true;
		case invalidCharacters::PIPE: return true;
		}
	}
	return false;
}