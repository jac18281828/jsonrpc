FROM jac18281828/cpp:latest

ARG PROJECT=crucible
WORKDIR /workspaces/${PROJECT}


COPY --chown=cpp:cpp . .
USER cpp

ENV PATH=${PATH}:/usr/local/go/bin
#RUN yamlfmt -lint .github/workflows/*.yml

ARG BUILD=build
ARG TYPE=DEBUG
RUN mkdir -p ${BUILD}
#build
ENV MAKEFLAGS=-j8
#RUN cmake -H. -B${BUILD} -DPROJECT_NAME=${PROJECT} -DCMAKE_BUILD_TYPE=${TYPE} -DCMAKE_VERBOSE_MAKEFILE=on -Wno-dev "-GUnix Makefiles"
#RUN cmake --build ${BUILD} --verbose --config ${TYPE}
#ENV CTEST_OUTPUT_ON_FAILURE=1
#RUN (cd ${BUILD} && ctest)
#ENV PROJECT_NAME=${PROJECT}
#RUN valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind.log build/main
#CMD build/main
