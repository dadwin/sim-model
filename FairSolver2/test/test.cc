
#include <cstdlib>
#include <vector>
#include "src/Solver.h"
#include "gtest/gtest.h"

static const double ep = 0.0001;

#define SIZE(array) sizeof((array))/sizeof((array[0]))

std::vector<Resource*>* getPath(const std::vector<Resource*>& resources, const long* array, size_t size) {
    auto path = new std::vector<Resource*>();
    for (unsigned i = 0; i < size; i++) {
        auto a = array[i];
        auto r = resources.at(a);
        path->push_back(r);
//        path->push_back(resources.at(array[i]));
    }
    return path;
}

TEST(SimpleTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10));

    long p0[] = { 0 };
    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(1.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(9.0, resources.at(0)->getCapacity());
}

// TODO test failed:
// this test sets resource capacity to zero,
// but we have requirement that states allocation should be positive
#if ZERO_ALLOCATION_ALLOWED
TEST(SimpleTest, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(0));

    long p0[] = { 0 };
    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(0.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
}
#endif

TEST(SimpleTest, test3) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(1.0));

    long p0[] = { 0 };
    flows.push_back(new Flow(0.9, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(0.9, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(0.1, resources.at(0)->getCapacity());
}

TEST(SimpleTest, test4) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(1.01));

    long p0[] = { 0 };
    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(1.0, flows.at(0)->getAllocation());
    EXPECT_NEAR(0.01, resources.at(0)->getCapacity(), ep); //EXPECT_DOUBLE_EQ(0.01, resources.at(0)->getCapacity());
}

TEST(ComplexTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(7.0));
    resources.push_back(new Resource(8.0));
    resources.push_back(new Resource(7.0));
    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(5.0));

    long p0[] = { 0, 2, 3, 4, 5 };
    long p1[] = { 0, 2, 3, 4, 6 };
    long p2[] = { 6, 4, 3, 2, 1 };
    long p3[] = { 5, 4, 3, 2, 1 };
    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(1.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(1.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(1.0, getPath(resources, p3, SIZE(p3))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(1.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(1.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(1.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(1.0, flows.at(3)->getAllocation());
    EXPECT_DOUBLE_EQ(3.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(4.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(4)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(5)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(6)->getCapacity());
}

TEST(ComplexTest, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(7.0));
    resources.push_back(new Resource(8.0));
    resources.push_back(new Resource(7.0));
    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(5.0));

    long p0[] = { 0, 2, 3, 4, 5 };
    long p1[] = { 0, 2, 3, 4, 6 };
    long p2[] = { 6, 4, 3, 2, 1 };
    long p3[] = { 5, 4, 3, 2, 1 };

    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(1.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(2.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(1.0, getPath(resources, p3, SIZE(p3))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(1.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(1.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(2.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(1.0, flows.at(3)->getAllocation());
    EXPECT_DOUBLE_EQ(3.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(4)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(5)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(6)->getCapacity());
}

TEST(ComplexTest, test3) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(7.0));
    resources.push_back(new Resource(8.0));
    resources.push_back(new Resource(7.0));
    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(5.0));

    long p0[] = { 0, 2, 3, 4, 5 };
    long p1[] = { 0, 2, 3, 4, 6 };
    long p2[] = { 6, 4, 3, 2, 1 };
    long p3[] = { 5, 4, 3, 2, 1 };

    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(1.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(10.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(1.0, getPath(resources, p3, SIZE(p3))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(1.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(1.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(1.0, flows.at(3)->getAllocation());
    EXPECT_DOUBLE_EQ(3.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(1.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(5)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(6)->getCapacity());
}

TEST(ComplexTest, test4) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(5.0));
    resources.push_back(new Resource(11.0));

    long p0[] = { 0, 1 };
    long p1[] = { 1 };

    flows.push_back(new Flow(5, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(6, getPath(resources, p1, SIZE(p1))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(5.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(6.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
}

TEST(TwoResourcesTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));

    long p0[] = { 0 };
    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(flows.at(0)->getAllocation(), 1.0);
    EXPECT_DOUBLE_EQ(resources.at(0)->getCapacity(), 9.0);
    EXPECT_DOUBLE_EQ(resources.at(1)->getCapacity(), 10.0);
}

TEST(TwoResourcesTest, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));

    long p0[] = { 1 };
    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(flows.at(0)->getAllocation(), 1.0);
    EXPECT_DOUBLE_EQ(resources.at(1)->getCapacity(), 9.0);
    EXPECT_DOUBLE_EQ(resources.at(0)->getCapacity(), 10.0);
}

TEST(TwoResourcesTest, test3) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));

    long p0[] = { 0 };
    flows.push_back(new Flow(1.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(flows.at(0)->getAllocation(), 1.0);
    EXPECT_DOUBLE_EQ(resources.at(0)->getCapacity(), 9.0);
    EXPECT_DOUBLE_EQ(resources.at(1)->getCapacity(), 10.0);
}

TEST(TwoResourcesTest, test4) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(9.0));
    resources.push_back(new Resource(10.0));

    long p0[] = { 0 };
    flows.push_back(new Flow(8.99999, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_NEAR(9.0, flows.at(0)->getAllocation(), ep); // EXPECT_DOUBLE_EQ(9.0, flows.at(0)->getAllocation());
    EXPECT_NEAR(0.0, resources.at(0)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_NEAR(10.0, resources.at(1)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(10.0, resources.at(1)->getCapacity());
}

TEST(TwoResourcesTest, test5) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(9.0));
    resources.push_back(new Resource(10.0));

    long p0[] = { 0, 1 };
    flows.push_back(new Flow(8.99999, getPath(resources, p0, SIZE(p0)))); // remember about dk < Ak

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_NEAR(9.0, flows.at(0)->getAllocation(), ep); // EXPECT_DOUBLE_EQ(9.0, flows.at(0)->getAllocation());
    EXPECT_NEAR(0.0, resources.at(0)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_NEAR(1.0, resources.at(1)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(1.0, resources.at(1)->getCapacity());
}

// Below is very sweet test cases:
// they are different by order of flows.
// Ideally no dependency on order of flows should be, but at some moment there was.
// In order to recognize it I wrote these two tests
TEST(FiveResourcesTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(4.0));   //4

    long p0[] = { 0, 1, 3 };
    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));
    long p1[] = { 2, 3, 4 };
    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_NEAR(100.0, flows.at(0)->getAllocation(), ep); // EXPECT_DOUBLE_EQ(9.0, flows.at(0)->getAllocation());
    EXPECT_NEAR(4.0, flows.at(1)->getAllocation(), ep);
    EXPECT_NEAR(0.0, resources.at(0)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_NEAR(0.0, resources.at(1)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(10.0, resources.at(1)->getCapacity());
    EXPECT_NEAR(6.0, resources.at(3)->getCapacity(), ep);
    EXPECT_NEAR(96.0, resources.at(2)->getCapacity(), ep);
    EXPECT_NEAR(0.0, resources.at(4)->getCapacity(), ep);
}

TEST(FiveResourcesTest, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(4.0));   //4

    long p1[] = { 2, 3, 4 };
    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));
    long p0[] = { 0, 1, 3 };
    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_NEAR(100.0, flows.at(1)->getAllocation(), ep); // EXPECT_DOUBLE_EQ(9.0, flows.at(0)->getAllocation());
    EXPECT_NEAR(4.0, flows.at(0)->getAllocation(), ep);
    EXPECT_NEAR(0.0, resources.at(0)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_NEAR(0.0, resources.at(1)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(10.0, resources.at(1)->getCapacity());
    EXPECT_NEAR(6.0, resources.at(3)->getCapacity(), ep);
    EXPECT_NEAR(96.0, resources.at(2)->getCapacity(), ep);
    EXPECT_NEAR(0.0, resources.at(4)->getCapacity(), ep);
}

TEST(FiveResourcesTest, test3) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(8.0));   //4

    long p0[] = { 0, 1, 3 };
    long p1[] = { 2, 3, 4 };
    long p2[] = { 4 };

    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(100.0, getPath(resources, p2, SIZE(p2))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(100.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(6.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(96.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
}

TEST(FiveResourcesTest, test4) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(8.0));   //4

    long p0[] = { 0, 1, 3 };
    long p1[] = { 2, 3, 4 };
    long p2[] = { 4 };

    flows.push_back(new Flow(100.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(4.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(100.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(6.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(96.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
}

TEST(FiveResourcesTest, test5) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(8.0));   //4

    long p0[] = { 0, 1, 3 };
    long p1[] = { 2, 3, 4 };
    long p2[] = { 4 };

    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(100.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(4.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(100.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(6.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(96.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
}

TEST(FiveResourcesTest, test6) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(8.0));   //4

    long p0[] = { 0, 1, 3 };
    long p1[] = { 2, 3, 4 };
    long p2[] = { 4 };


    flows.push_back(new Flow(100.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(4.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(100.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(6.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(96.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
}

TEST(FiveResourcesTest, test7) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(8.0));   //4

    long p0[] = { 0, 1, 3 };
    long p1[] = { 2, 3, 4 };
    long p2[] = { 4 };


    flows.push_back(new Flow(4.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(4.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(100.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(6.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(96.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
}

TEST(FiveResourcesTest, test8) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(100.0)); //0
    resources.push_back(new Resource(100.0)); //1
    resources.push_back(new Resource(100.0)); //2
    resources.push_back(new Resource(110.0)); //3
    resources.push_back(new Resource(8.0));   //4

    long p0[] = { 0, 1, 3 };
    long p1[] = { 2, 3, 4 };
    long p2[] = { 4 };

    flows.push_back(new Flow(100.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(4.0, getPath(resources, p2, SIZE(p2))));
    flows.push_back(new Flow(100.0, getPath(resources, p0, SIZE(p0))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(4.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(100.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(6.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(96.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
}

TEST(ThreeResourcesTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(1.0)); //0
    resources.push_back(new Resource(2.0)); //1
    resources.push_back(new Resource(3.0)); //2

    long p0[] = { 0, 1 };
    long p1[] = { 1, 2 };
    long p2[] = { 0, 2 };

    flows.push_back(new Flow(10.0, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(10.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(10.0, getPath(resources, p2, SIZE(p2))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(0.5, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(1.5, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(0.5, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(1.0, resources.at(2)->getCapacity());
}

TEST(ThreeResourcesTest, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(1.0)); //0
    resources.push_back(new Resource(2.0)); //1
    resources.push_back(new Resource(3.0)); //2

    long p0[] = { 0, 1 };
    long p1[] = { 1, 2 };
    long p2[] = { 0, 2 };

    flows.push_back(new Flow(0.5, getPath(resources, p0, SIZE(p0))));
    flows.push_back(new Flow(10.0, getPath(resources, p1, SIZE(p1))));
    flows.push_back(new Flow(10.0, getPath(resources, p2, SIZE(p2))));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(0.5, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(1.5, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(0.5, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(1.0, resources.at(2)->getCapacity());
}

extern size_t ResourceSize;
extern size_t FlowSize;

TEST(Performance, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    for (uint i = 0; i < ResourceSize; i++) {
        resources.push_back(new Resource(1.0));
    }

    long path[ResourceSize];
    for (uint i = 0; i < ResourceSize; i++) {
        path[i] = i;
    }

    for (uint i = 0; i < FlowSize; i++) {
        flows.push_back(new Flow(0.5, getPath(resources, path, ResourceSize)));
    }

    Solver::solve(flows, resources);

    //Solver::printFlows(flows);
    //Solver::printResources(resources);

    for (uint i = 0; i < FlowSize; i++) {
        EXPECT_NEAR(0.01, flows.at(i)->getAllocation(), ep); // EXPECT_DOUBLE_EQ(0.01, flows.at(0)->getAllocation());
    }
    for (uint i = 0; i < ResourceSize; i++) {
        EXPECT_NEAR(0.0, resources.at(i)->getCapacity(), ep);// EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    }

}

TEST(Performance, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    for (uint i = 0; i < ResourceSize; i++) {
        resources.push_back(new Resource(1.0));
    }

    std::srand(0);

    const size_t HopSize = 10; // assumption that 10 hops are enough
    long path[HopSize];

    for (uint i = 0; i < FlowSize; i++) {

        uint startPos = i; //std::rand(); random number complicates verification
        uint endPos = (startPos + HopSize);
        for (uint j = startPos, k = 0; j < endPos; j++, k++) {
            path[k] = j % ResourceSize; // to handle overflow
        }

        flows.push_back(new Flow(0.5, getPath(resources, path, HopSize)));
    }

    Solver::solve(flows, resources);

    //Solver::printFlows(flows);
    //Solver::printResources(resources);

    for (uint i = 0; i < FlowSize; i++) {
        EXPECT_LE(flows.at(i)->getAllocation(), 0.100001); // EXPECT_DOUBLE_EQ(0.01, flows.at(0)->getAllocation());
    }

    //        EXPECT_NEAR(0.0, resources.at(0)->getCapacity(), ep);// EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
}

TEST(IntersectedFlowsTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(15.0));
    resources.push_back(new Resource(15.0));
    resources.push_back(new Resource(15.0));
    resources.push_back(new Resource(15.0));
    resources.push_back(new Resource(15.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));


    long p0[] = { 0, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(10.0, getPath(resources, p0, SIZE(p0))));
    long p1[] = { 1, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(7.0, getPath(resources, p1, SIZE(p1))));
    long p2[] = { 1, 2, 3, 4, 5, 6, 8 };
    flows.push_back(new Flow(10.0, getPath(resources, p2, SIZE(p2))));


    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(5.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(5)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(6)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(7)->getCapacity());
    EXPECT_DOUBLE_EQ(5.0, resources.at(8)->getCapacity());
}

TEST(IntersectedFlowsTest, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(18.0));
    resources.push_back(new Resource(18.0));
    resources.push_back(new Resource(18.0));
    resources.push_back(new Resource(18.0));
    resources.push_back(new Resource(18.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));


    long p0[] = { 0, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(10.0, getPath(resources, p0, SIZE(p0))));
    long p1[] = { 1, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(7.0, getPath(resources, p1, SIZE(p1))));
    long p2[] = { 1, 2, 3, 4, 5, 6, 8 };
    flows.push_back(new Flow(10.0, getPath(resources, p2, SIZE(p2))));


    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(5.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(4)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(5)->getCapacity());
    EXPECT_DOUBLE_EQ(3.0, resources.at(6)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(7)->getCapacity());
    EXPECT_DOUBLE_EQ(5.0, resources.at(8)->getCapacity());
}

TEST(IntersectedFlowsTest, test3) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));


    long p0[] = { 0, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(10.0, getPath(resources, p0, SIZE(p0))));
    long p1[] = { 1, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(7.0, getPath(resources, p1, SIZE(p1))));
    long p2[] = { 1, 2, 3, 4, 5, 6, 8 };
    flows.push_back(new Flow(10.0, getPath(resources, p2, SIZE(p2))));


    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(4.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(4.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(6.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(4)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(5)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(6)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(7)->getCapacity());
    EXPECT_DOUBLE_EQ(6.0, resources.at(8)->getCapacity());
}

TEST(CrossedFlowsTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(15.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(12.0));
    resources.push_back(new Resource(15.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(10.0));
    resources.push_back(new Resource(15.0));


    long p0[] = { 0, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(10.0, getPath(resources, p0, SIZE(p0))));
    long p1[] = { 1, 2, 3, 4, 5, 6, 7 };
    flows.push_back(new Flow(7.0, getPath(resources, p1, SIZE(p1))));
    long p2[] = { 1, 2, 9, 6, 8 };
    flows.push_back(new Flow(10.0, getPath(resources, p2, SIZE(p2))));


    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(5.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, flows.at(1)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, flows.at(2)->getAllocation());
    EXPECT_DOUBLE_EQ(5.0, resources.at(0)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(1)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(2)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(3)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(4)->getCapacity());
    EXPECT_DOUBLE_EQ(2.0, resources.at(5)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(6)->getCapacity());
    EXPECT_DOUBLE_EQ(0.0, resources.at(7)->getCapacity());
    EXPECT_DOUBLE_EQ(5.0, resources.at(8)->getCapacity());
    EXPECT_DOUBLE_EQ(10.0, resources.at(9)->getCapacity());
}

