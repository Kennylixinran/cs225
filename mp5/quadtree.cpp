/**
 * @file quadtree.cpp
 * Quadtree class implementation.
 * @date Spring 2008
 */
 #include "quadtree.h"
 #include <math.h>
 using namespace std;
Quadtree::~Quadtree(){
	clear();
}
void Quadtree::clear(){
	clear(root);
}
void Quadtree::clear(QuadtreeNode* &croot){
	if(croot == NULL)
		return;
	clear(croot->nwChild);
	clear(croot->neChild);
	clear(croot->swChild);
	clear(croot->seChild);
	delete croot;
	croot = NULL;
}
Quadtree::Quadtree(Quadtree const &other){
	width = other.width;
	copyHelp(root, other.root);
}
Quadtree const & Quadtree::operator=(Quadtree const & other){
	clear();
	this->width = other.width;
	copyHelp(this->root, other.root);
	return *this;
}	

void Quadtree::copy(Quadtree other){
	//copyHelp(root, other.root);
}

void Quadtree::copyHelp(QuadtreeNode*& croot, QuadtreeNode* oroot){
	if(oroot == NULL)
		return;
	croot = new QuadtreeNode();
	//croot = oroot;
	croot->element = oroot->element;
	copyHelp(croot->nwChild, oroot->nwChild);
	copyHelp(croot->neChild, oroot->neChild);
	copyHelp(croot->swChild, oroot->swChild);
	copyHelp(croot->seChild, oroot->seChild);
}

Quadtree::Quadtree(){
	root = NULL;
}
Quadtree::Quadtree(PNG const &img, int d){
	QuadtreeNode* q = buildTreeHelp(img, 0, 0, d);
	root = q;
	width = d;
	//cout<<getNumberLeaves(root)<<endl;
}
void Quadtree::buildTree(PNG const &img, int d){
	clear();
	QuadtreeNode* q = buildTreeHelp(img, 0, 0, d);
	root = q;
	width = d;
	//leaves = d * d;
	//printTree();
}

Quadtree::QuadtreeNode* 
Quadtree::buildTreeHelp(PNG const &img, int x, int y, int d){
	if(d == 1){
		QuadtreeNode* q = new QuadtreeNode();
		q->element = *img(x,y);
		q->nwChild = NULL;
		q->neChild = NULL;
		q->swChild = NULL;
    	q->seChild = NULL;
    	q->leaves = 0;
    	return q;
	}
	QuadtreeNode* q = new QuadtreeNode();

	q->nwChild = buildTreeHelp(img, x, y, d/2);
	q->neChild = buildTreeHelp(img, x + d/2, y, d/2);
	q->swChild = buildTreeHelp(img, x, y + d/2, d/2);
    q->seChild = buildTreeHelp(img, x + d/2, y + d/2, d/2);
	q->element = RGBAPixel((q->nwChild->element.red + q->neChild->element.red + 
	q->swChild->element.red + q->seChild->element.red)/4, (q->nwChild->element.green + q->neChild->element.green + 
	q->swChild->element.green + q->seChild->element.green)/4, (q->nwChild->element.blue + q->neChild->element.blue + 
	q->swChild->element.blue + q->seChild->element.blue)/4);
	q->leaves = d*d;
	return q;
}

RGBAPixel Quadtree::getPixel(int x, int y)const{
	if(root == NULL)
		return RGBAPixel();
	return getPixelHelp(root, x, y, width)->element;
}
Quadtree::QuadtreeNode* 
Quadtree::getPixelHelp(QuadtreeNode* croot, int x, int y, int d)const{
	if(d <= 1 || (croot->nwChild == NULL && croot->neChild == NULL 
	&& croot->swChild == NULL && croot->seChild == NULL))
		return croot;
	if(x < d/2 && y < d/2 )
		return getPixelHelp(croot->nwChild, x, y, d/2);
	if(x < d/2  && y >= d/2)
		return getPixelHelp(croot->swChild, x, y - d/2, d/2);
	if(x >= d/2  && y < d/2 )
		return getPixelHelp(croot->neChild, x - d/2, y, d/2);
	else
		return getPixelHelp(croot->seChild, x - d/2, y - d/2, d/2);
}
int Quadtree::getNumberLeaves(QuadtreeNode* croot){
	if(croot->nwChild == NULL && croot->neChild == NULL && 
		croot->swChild == NULL && croot->seChild == NULL)
		return 1;
	return getNumberLeaves(croot->nwChild) + getNumberLeaves(croot->neChild)
	+ getNumberLeaves(croot->swChild) + getNumberLeaves(croot->seChild);
}
PNG Quadtree::decompress()const{
	if(root == NULL)
		return PNG();
	// int d = width;
	PNG pic(width, width);
	for(int i = 0; i < width; i++){
		for(int j = 0; j < width; j++){
			RGBAPixel r = getPixel(i, j);
			*pic(i, j) = r;
		}
	}
	// decompressHelp(pic, 0, 0);
	return pic;
}

void Quadtree::decompressHelp(PNG &img, int x, int y){
	if(x >= width || y >= width)
		return;
	cout<<"x = "<<x<<"y = "<<y<<endl;
	RGBAPixel r = getPixel(x, y);
	*img(x, y) = r;
	decompressHelp(img, x + 1, y);
	decompressHelp(img, x, y + 1);
}



void Quadtree::clockwiseRotate(){
	if(root == NULL)
		return;
	rotateHelp(root);
}
void Quadtree::rotateHelp(QuadtreeNode* croot){
	if(croot == NULL)
		return;
	QuadtreeNode* p1 = croot->nwChild;
	QuadtreeNode* p2 = croot->neChild;
	QuadtreeNode* p3 = croot->swChild;
	QuadtreeNode* p4 = croot->seChild;
	croot->neChild = p1;
	croot->seChild = p2;
	croot->swChild = p4;
	croot->nwChild = p3;
	rotateHelp(croot->nwChild);
	rotateHelp(croot->neChild);
	rotateHelp(croot->swChild);
	rotateHelp(croot->seChild);
}

void Quadtree::prune(int tolerance){
	pruneHelp(root, tolerance);
}

void Quadtree::pruneHelp(QuadtreeNode* croot, int t){
	if(croot==NULL)
		return;
	if(checkPrune(croot, croot, t)){
		clear(croot->nwChild);
		clear(croot->neChild);
		clear(croot->swChild);
		clear(croot->seChild);
		return;
	}
	pruneHelp(croot->swChild, t);
	pruneHelp(croot->seChild, t);
	pruneHelp(croot->nwChild, t);
	pruneHelp(croot->neChild, t);
}

bool Quadtree::checkPrune(QuadtreeNode* croot1, QuadtreeNode* croot2, int t)const{
	if(croot2->nwChild == NULL){
		if(prunable(croot1->element, croot2->element, t))
			return true;
		return false;
	}
	if(checkPrune(croot1, croot2->nwChild, t) && checkPrune(croot1, croot2->neChild, t)
		&& checkPrune(croot1, croot2->swChild, t) && checkPrune(croot1, croot2->seChild, t)) 
		return true;
	return false;
}

bool Quadtree::prunable(RGBAPixel r1, RGBAPixel r2, int t)const{
	uint8_t red1 = r1.red;
	uint8_t green1 = r1.green;
	uint8_t blue1 = r1.blue;

	uint8_t red2 = r2.red;
	uint8_t green2 = r2.green;
	uint8_t blue2 = r2.blue;

	if(((red1 - red2)*(red1 - red2) + (green1 - green2)*(green1 - green2) + 
	(blue1 - blue2)*(blue1 - blue2)) <= t)
		return true;
	return false;
}

int Quadtree::pruneSize(int tolerance)const{
	int NumOfleaves = root->leaves;
	pruneSizeHelp(root, tolerance, NumOfleaves);
	return NumOfleaves;
}

void Quadtree::pruneSizeHelp(QuadtreeNode* croot, int t, int & l)const{
	if(croot->nwChild == NULL && croot->neChild == NULL && 
	croot->swChild == NULL && croot->seChild == NULL)
		return;
	if(checkPrune(croot, croot, t)){
		l = l - croot->leaves + 1;
		return;
	}
	pruneSizeHelp(croot->nwChild, t, l);
	pruneSizeHelp(croot->neChild, t, l);
	pruneSizeHelp(croot->swChild, t, l);
	pruneSizeHelp(croot->seChild, t, l);
}

int Quadtree::idealPrune(int NumOfleaves)const{
	int max = 255*255 + 255*255 + 255*255;
	int min = 0;
	int result = idealPruneHelp(max, min, (min + max)/2, NumOfleaves);
	if(pruneSize(result) == NumOfleaves)
		return result;
	return result + 1;
}
int Quadtree::idealPruneHelp(int max, int min, int n, int l)const{
	// cout<<"n = "<<n;
	// cout<<" min = "<<min;
	// cout<<" max = "<<max;
	// cout<<" pruneSize = "<<pruneSize(n)<<endl;
	if(max == min || min == n || max == n)
		return n;
	if(pruneSize(n) <= l){
		return idealPruneHelp(n, min, (n + min)/2, l);
	}
	else{
		return idealPruneHelp(max, n, (max + n)/2, l);
	}
	// else
	// 	return idealPruneHelp();
}

