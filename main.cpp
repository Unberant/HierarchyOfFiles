#include "FilesHierarchyPattern.h"
//namespace  fs = std::filesystem;
//using std::endl;
//using std::cout;
//using namespace std::chrono;

int main()
{
	//iBaseFileComponent* tree = new eFolderComposite("root");
	//tree->printLastModificateTime();
	//iBaseFileComponent* br1 = new eFolderComposite("root/", "br1");
	//br1->printLastModificateTime();
	//iBaseFileComponent* fil1 = new eFileLeaf("root/", "abc.tnt");
	//fil1->printLastModificateTime();

	eElement tree1(new eFolderComposite("root1"));
	eElement branch1(new eFolderComposite("root1/branch1"));
	eElement branch2(new eFolderComposite("root1/", "branch2"));
	eElement branch3(new eFolderComposite("root1/branch2/", "branch3"));

	eElement leaf1(new eFileLeaf("root1/", "leaf1.txt"));
	eElement leaf2(new eFileLeaf("root1/branch1/", "leaf2.h"));
	eElement leaf3(new eFileLeaf("root1/branch1/", "leaf3.c"));
	eElement leaf4(new eFileLeaf("root1/branch2/", "leaf4.cpp"));

	tree1.addConnection(branch1);
	branch1.addConnection(leaf2);
	branch1.addConnection(leaf3);

	tree1.addConnection(leaf1);

	tree1.addConnection(branch2);
	branch2.addConnection(branch3);
	branch2.addConnection(leaf4);

	tree1.printDescription();
	std::cout << "\n============\n";

	//eElement branch22 = branch2.getObject("root1/leaf1.txt"); // 1
	//eElement leaf11 = leaf1.getObject("root1/leaf1.txt"); // 3

	//std::cout << tree1.getRootFolder() << std::endl; // 3 
	//std::cout << branch1.getRootFolder() << std::endl;
	//std::cout << branch2.getRootFolder() << std::endl;
	//std::cout << leaf1.getRootFolder() << std::endl;
	//std::cout << leaf4.getRootFolder() << std::endl;

	//std::cout << std::boolalpha; // 4
	//std::cout << "branch1[folder] is folder. it's " << branch1.isFolder() << std::endl;
	//std::cout << "leaf3[leaf3.c] is folder. it's " << leaf3.isFolder() << std::endl;
	//std::cout << std::noboolalpha;

	//std::cout << "branch1[folder] name: " << branch1.getName() << std::endl; // 5.1
	//std::cout << "leaf1[leaf.txt] name: " << leaf1.getName() << std::endl;
	//std::cout << "branch1[folder] creation date: " << branch1.getCreationDate() << std::endl; // 5.2
	//std::cout << "branch1[folder] last modification date: " << branch1.getLastModificationDate() << std::endl; // 5.3
	//std::cout << "leaf1[leaf.txt] creation date: " << leaf1.getCreationDate() << std::endl;
	//std::cout << "tree1 size (size of all files in this folder): " << tree1.getSizeInBytes() << std::endl; // 5.4
	//std::cout << "leaf1 size: " << leaf1.getSizeInBytes() << std::endl; // 5.4
	//std::cout << "in tree1 " << tree1.getNumberOfFiles() << std::endl; // 5.5
	//std::cout << "in branch2 " << branch2.getNumberOfFiles() << std::endl; // 5.5

	//eElement branch4 = tree1.createFolder("root1/", "branch4"); // 6
	//eElement leaf5 = branch4.creteFile("root1/branch4/", "leaf.c", 10); // 7
	//std::cout << "\n============\n";
	//tree1.printDescription();
	//std::cout << "\n============\n";
	//branch4.printDescription();

	//leaf1.modifyFileSize(10); // 8
	//leaf1.printCreationDate();
	//leaf1.printLastModificationDate();

	//std::cout << " \n[Starting Situation]\n"; // 9
	//tree1.printDescription();
	//tree1.deleteConnection(leaf4);
	//std::cout << " \n[Delete connection with leaf4, but file still exist]\n";
	//tree1.printDescription();
	//tree1.removeFileFromSystem(branch3);
	//std::cout << "\n[delete branch4, Delete connection with branch, Delete folder from computer]\n";
	//tree1.printDescription();

	//branch1.rename("branch1_new"); // 10
	//leaf1.rename("not.txt.file.tnt");
	//tree1.printDescription();

	leaf4.removeFileFromSystem(tree1);
	tree1.printDescription();


}

