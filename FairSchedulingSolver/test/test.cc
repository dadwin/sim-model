
#include <cstdlib>
#include "solver/Solver.h"
#include "gtest/gtest.h"

static const double ep = 0.0001;

#define SIZE(array) sizeof((array))/sizeof((array[0]))
#if 0
TEST(SimpleTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(0, 10));

    std::vector<long> path(1);
    path[0] = 0;
    flows.push_back(new Flow(0, 1, path));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(1.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(9.0, resources.at(0)->getCapacity());
}

TEST(SimpleTest, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(0, 0));

    std::vector<long> path(1);
    path[0] = 0;
    flows.push_back(new Flow(0, 1, path));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(0.0, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
}

TEST(SimpleTest, test3) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(0, 1.0));

    std::vector<long> path(1);
    path[0] = 0;
    flows.push_back(new Flow(0, 0.9, path));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(0.9, flows.at(0)->getAllocation());
    EXPECT_DOUBLE_EQ(0.1, resources.at(0)->getCapacity());
}

TEST(SimpleTest, test4) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(0, 1.01));

    std::vector<long> path(1);
    path[0] = 0;
    flows.push_back(new Flow(0, 1.0, path));

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_DOUBLE_EQ(1.0, flows.at(0)->getAllocation());
    EXPECT_NEAR(0.01, resources.at(0)->getCapacity(), ep); //EXPECT_DOUBLE_EQ(0.01, resources.at(0)->getCapacity());
}

TEST(ComplexTest, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    resources.push_back(new Resource(0, 5.0));
    resources.push_back(new Resource(1, 5.0));
    resources.push_back(new Resource(2, 7.0));
    resources.push_back(new Resource(3, 8.0));
    resources.push_back(new Resource(4, 7.0));
    resources.push_back(new Resource(5, 5.0));
    resources.push_back(new Resource(6, 5.0));

    long p0[] = { 0, 2, 3, 4, 5 };
    long p1[] = { 0, 2, 3, 4, 6 };
    long p2[] = { 6, 4, 3, 2, 1 };
    long p3[] = { 5, 4, 3, 2, 1 };
    flows.push_back(new Flow(0, 1.0, p0, SIZE(p0)));
    flows.push_back(new Flow(1, 1.0, p1, SIZE(p1)));
    flows.push_back(new Flow(2, 1.0, p2, SIZE(p2)));
    flows.push_back(new Flow(3, 1.0, p3, SIZE(p3)));

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

    resources.push_back(new Resource(0, 5.0));
    resources.push_back(new Resource(1, 5.0));
    resources.push_back(new Resource(2, 7.0));
    resources.push_back(new Resource(3, 8.0));
    resources.push_back(new Resource(4, 7.0));
    resources.push_back(new Resource(5, 5.0));
    resources.push_back(new Resource(6, 5.0));

    long p0[] = { 0, 2, 3, 4, 5 };
    long p1[] = { 0, 2, 3, 4, 6 };
    long p2[] = { 6, 4, 3, 2, 1 };
    long p3[] = { 5, 4, 3, 2, 1 };

    flows.push_back(new Flow(0, 1.0, p0, SIZE(p0)));
    flows.push_back(new Flow(1, 1.0, p1, SIZE(p1)));
    flows.push_back(new Flow(2, 2.0, p2, SIZE(p2)));
    flows.push_back(new Flow(3, 1.0, p3, SIZE(p3)));

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

    resources.push_back(new Resource(0, 5.0));
    resources.push_back(new Resource(1, 5.0));
    resources.push_back(new Resource(2, 7.0));
    resources.push_back(new Resource(3, 8.0));
    resources.push_back(new Resource(4, 7.0));
    resources.push_back(new Resource(5, 5.0));
    resources.push_back(new Resource(6, 5.0));

    long p0[] = { 0, 2, 3, 4, 5 };
    long p1[] = { 0, 2, 3, 4, 6 };
    long p2[] = { 6, 4, 3, 2, 1 };
    long p3[] = { 5, 4, 3, 2, 1 };

    flows.push_back(new Flow(0, 1.0, p0, SIZE(p0)));
    flows.push_back(new Flow(1, 1.0, p1, SIZE(p1)));
    flows.push_back(new Flow(2, 10.0, p2, SIZE(p2)));
    flows.push_back(new Flow(3, 1.0, p3, SIZE(p3)));

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

    resources.push_back(new Resource(0, 5.0));
    resources.push_back(new Resource(1, 11.0));

    long p0[] = { 0, 1 };
    long p1[] = { 1 };

    flows.push_back(new Flow(0, 5, p0, SIZE(p0)));
    flows.push_back(new Flow(1, 6, p1, SIZE(p1)));

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

    resources.push_back(new Resource(0, 10.0));
    resources.push_back(new Resource(1, 10.0));

    long p0[] = { 0 };
    flows.push_back(new Flow(0, 1.0, p0, SIZE(p0)));

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

    resources.push_back(new Resource(0, 10.0));
    resources.push_back(new Resource(1, 10.0));

    long p0[] = { 1 };
    flows.push_back(new Flow(0, 1.0, p0, SIZE(p0)));

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

    resources.push_back(new Resource(0, 10.0));
    resources.push_back(new Resource(1, 10.0));

    long p0[] = { 0 };
    flows.push_back(new Flow(0, 1.0, p0, SIZE(p0)));

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

    resources.push_back(new Resource(0, 9.0));
    resources.push_back(new Resource(1, 10.0));

    long p0[] = { 0 };
    flows.push_back(new Flow(0, 8.99999, p0, SIZE(p0)));

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

    resources.push_back(new Resource(0, 9.0));
    resources.push_back(new Resource(1, 10.0));

    long p0[] = { 0, 1 };
    flows.push_back(new Flow(0, 8.99999, p0, SIZE(p0))); // remember about dk < Ak

    Solver::solve(flows, resources);

    Solver::printFlows(flows);
    Solver::printResources(resources);

    EXPECT_NEAR(9.0, flows.at(0)->getAllocation(), ep); // EXPECT_DOUBLE_EQ(9.0, flows.at(0)->getAllocation());
    EXPECT_NEAR(0.0, resources.at(0)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());
    EXPECT_NEAR(1.0, resources.at(1)->getCapacity(), ep); // EXPECT_DOUBLE_EQ(1.0, resources.at(1)->getCapacity());
}
#endif

extern size_t ResourceSize;
extern size_t FlowSize;

#if 1
TEST(Performance, test1) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    for (uint i = 0; i < ResourceSize; i++) {
        resources.push_back(new Resource(i, 1.0));
    }

    long path[ResourceSize];
    for (uint i = 0; i < ResourceSize; i++) {
        path[i] = i;
    }

    for (uint i = 0; i < FlowSize; i++) {
        flows.push_back(new Flow(i, 0.5, path, ResourceSize));
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
#else
TEST(Performance, test2) {
    std::vector<Flow*> flows;
    std::vector<Resource*> resources;

    for (uint i = 0; i < ResourceSize; i++) {
        resources.push_back(new Resource(i, 1.0));
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

        flows.push_back(new Flow(i, 0.5, path, HopSize));
    }

    Solver::solve(flows, resources);

    //Solver::printFlows(flows);
    //Solver::printResources(resources);

    for (uint i = 0; i < FlowSize; i++) {
        EXPECT_LE(flows.at(i)->getAllocation(), 0.100001); // EXPECT_DOUBLE_EQ(0.01, flows.at(0)->getAllocation());
    }

    //        EXPECT_NEAR(0.0, resources.at(0)->getCapacity(), ep);// EXPECT_DOUBLE_EQ(0.0, resources.at(0)->getCapacity());

}
#endif
