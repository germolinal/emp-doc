/* polygon_test.h */
#include "../src/common/geometry/polygon.h"


TEST(PolygonTest, emptyOnConstructor)
{
	Polygon3D p = Polygon3D();

	ASSERT_EQ(p.getArea(), -1);
	ASSERT_EQ(p.countInnerLoops(), 0);
	ASSERT_EQ(p.getOuterLoopRef()->size(), 0);
	ASSERT_TRUE(p.getNormal().isZero());
	ASSERT_FALSE(p.hasInnerLoops());
}

TEST(PolygonTest, getClosedLoop)
{
	Polygon3D p = Polygon3D();
	
	Loop * outerLoop = p.getOuterLoopRef();
	outerLoop->addVertex(new Point3D(-2, -2,0));
	outerLoop->addVertex(new Point3D( 6, -2, 0));
	outerLoop->addVertex(new Point3D( 6,  6, 0));
	outerLoop->addVertex(new Point3D(-2,  6, 0));
	
	p.addInnerLoop();
	Loop * innerLoop = p.getInnerLoopRef(0);
	innerLoop->addVertex(new Point3D(-1, -1, 0));
	innerLoop->addVertex(new Point3D( 1, -1, 0));
	innerLoop->addVertex(new Point3D( 1,  1, 0));
	innerLoop->addVertex(new Point3D(-1,  1, 0));

	Loop * closed = p.getClosedLoop();

	ASSERT_EQ(closed->size(), 10);
	
	// First one
	ASSERT_TRUE(closed->getVertexRef(0)->isEqual(Point3D(-2, -2, 0)));

	// interior loop
	ASSERT_TRUE(closed->getVertexRef(1)->isEqual(Point3D(-1, -1, 0)));
	ASSERT_TRUE(closed->getVertexRef(2)->isEqual(Point3D(-1, 1, 0)));
	ASSERT_TRUE(closed->getVertexRef(3)->isEqual(Point3D( 1, 1, 0)));
	ASSERT_TRUE(closed->getVertexRef(4)->isEqual(Point3D( 1, -1, 0)));
	ASSERT_TRUE(closed->getVertexRef(5)->isEqual(Point3D(-1, -1, 0)));
	
	// Back to exterior
	ASSERT_TRUE(closed->getVertexRef(6)->isEqual(Point3D(-2, -2, 0)));

	//Now, exterior
	ASSERT_TRUE(closed->getVertexRef(7)->isEqual(Point3D( 6, -2, 0)));
	ASSERT_TRUE(closed->getVertexRef(8)->isEqual(Point3D( 6,  6, 0)));
	ASSERT_TRUE(closed->getVertexRef(9)->isEqual(Point3D(-2,  6, 0)));
}


TEST(PolygonTest, getClosedLoopWithClean)
{
	Polygon3D p = Polygon3D();

	Loop * outerLoop = p.getOuterLoopRef();
	outerLoop->addVertex(new Point3D(-2, -2, 0));
	outerLoop->addVertex(new Point3D(0, -2, 0)); // colinear point
	outerLoop->addVertex(new Point3D(6, -2, 0));
	outerLoop->addVertex(new Point3D(6, 6, 0));
	outerLoop->addVertex(new Point3D(-2, 6, 0));

	p.addInnerLoop();
	Loop * innerLoop = p.getInnerLoopRef(0);
	innerLoop->addVertex(new Point3D(-1, -1, 0));
	innerLoop->addVertex(new Point3D(1, -1, 0));
	innerLoop->addVertex(new Point3D(1, 1, 0));
	innerLoop->addVertex(new Point3D(-1, 1, 0));

	p.clean();

	Loop * closed = p.getClosedLoop();

	ASSERT_EQ(closed->size(), 11);

	// First one
	int i = -1;
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(-2, -2, 0)));

	// interior loop
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(-1, -1, 0)));
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(-1, 1, 0)));
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(1, 1, 0)));
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(1, -1, 0)));
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(-1, -1, 0)));

	// Back to exterior
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(-2, -2, 0)));

	//Now, exterior
	ASSERT_TRUE(closed->getVertexRef(++i) == NULL );
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(6, -2, 0)));
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(6, 6, 0)));
	ASSERT_TRUE(closed->getVertexRef(++i)->isEqual(Point3D(-2, 6, 0)));
}


TEST(PolygonTest, testPoint)
{
	Polygon3D p = Polygon3D();

	p.setNormal(Vector3D(0, 0, 1));
	Loop * outerLoop = p.getOuterLoopRef();
	outerLoop->addVertex(new Point3D(-2, -2, 0));
	outerLoop->addVertex(new Point3D(2, -2, 0));
	outerLoop->addVertex(new Point3D(2, 2, 0));
	outerLoop->addVertex(new Point3D(-2, 2, 0));

	p.addInnerLoop();
	Loop * innerLoop = p.getInnerLoopRef(0);
	innerLoop->addVertex(new Point3D(-1, -1, 0));
	innerLoop->addVertex(new Point3D(1, -1, 0));
	innerLoop->addVertex(new Point3D(1, 1, 0));
	innerLoop->addVertex(new Point3D(-1, 1, 0));

	Point3D origin = Point3D(0, 0, 0);
	Point3D out = Point3D(-3, -3, 0);
	Point3D in = Point3D(-1.5, -1.5, 0);

	Vector3D * n = new Vector3D(0, 0, 1);
	ASSERT_TRUE(innerLoop->testPoint(origin,n));
	ASSERT_TRUE(outerLoop->testPoint(origin, n));
	ASSERT_FALSE(outerLoop->testPoint(out, n));
	ASSERT_FALSE(innerLoop->testPoint(out, n));
	ASSERT_TRUE(outerLoop->testPoint(in, n));
	ASSERT_FALSE(innerLoop->testPoint(in, n));


	ASSERT_FALSE(p.testPoint(origin)); // inside a hole
	ASSERT_FALSE(p.testPoint(out)); // outside outer loop
	ASSERT_TRUE(p.testPoint(in)); // Inside outer loop but out of holes
}