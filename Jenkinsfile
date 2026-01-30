pipeline {
    agent any
    
    environment {
        DOCKER_REGISTRY = 'registry.gitlab.com/meta-repo-architecture'
        DOCKER_CREDENTIALS = credentials('docker-registry-credentials')
        SONAR_TOKEN = credentials('sonar-token')
        SLACK_WEBHOOK = credentials('slack-webhook')
        GITHUB_TOKEN = credentials('github-token')
    }
    
    options {
        buildDiscarder(logRotator(numToKeepStr: '10'))
        timeout(time: 4, unit: 'HOURS')
        timestamps()
        ansiColor('xterm')
    }
    
    stages {
        stage('Setup') {
            parallel {
                stage('Linux Setup') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Setting up Linux build environment"
                            sudo apt-get update
                            sudo apt-get install -y cmake build-essential git clang clang-tidy valgrind doxygen graphviz plantuml python3
                        '''
                    }
                }
                stage('Windows Setup') {
                    agent {
                        label 'windows'
                    }
                    steps {
                        bat '''
                            echo "Setting up Windows build environment"
                            choco install cmake git
                            call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Auxiliary\\Build\\vcvars64.bat"
                        '''
                    }
                }
                stage('macOS Setup') {
                    agent {
                        label 'macos'
                    }
                    steps {
                        sh '''
                            echo "Setting up macOS build environment"
                            brew update
                            brew install cmake clang-format clang-tidy valgrind doxygen graphviz plantuml python3
                        '''
                    }
                }
            }
        }
        
        stage('Build') {
            parallel {
                stage('Linux Build') {
                    agent {
                        label 'linux'
                    }
                    matrix {
                        axes {
                            axis {
                                name 'COMPILER'
                                values 'gcc', 'clang'
                            }
                            axis {
                                name 'BUILD_TYPE'
                                values 'Debug', 'Release'
                            }
                        }
                        stages {
                            stage('Build Matrix') {
                                steps {
                                    sh """
                                        echo "Building with ${COMPILER} in ${BUILD_TYPE} mode"
                                        mkdir -p build-${COMPILER}-${BUILD_TYPE}
                                        cd build-${COMPILER}-${BUILD_TYPE}
                                        cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_CXX_COMPILER=${COMPILER}++
                                        make -j\$(nproc)
                                    """
                                    archiveArtifacts artifacts: "build-${COMPILER}-${BUILD_TYPE}/**/*", fingerprint: true
                                }
                            }
                        }
                    }
                }
                stage('Windows Build') {
                    agent {
                        label 'windows'
                    }
                    steps {
                        bat '''
                            echo "Building on Windows"
                            mkdir build-windows
                            cd build-windows
                            cmake .. -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release
                            cmake --build . --config Release --parallel
                        '''
                        archiveArtifacts artifacts: "build-windows/**/*", fingerprint: true
                    }
                }
                stage('macOS Build') {
                    agent {
                        label 'macos'
                    }
                    steps {
                        sh '''
                            echo "Building on macOS"
                            mkdir build-macos
                            cd build-macos
                            cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
                            make -j$(sysctl -n hw.ncpu)
                        '''
                        archiveArtifacts artifacts: "build-macos/**/*", fingerprint: true
                    }
                }
            }
        }
        
        stage('Test') {
            parallel {
                stage('Unit Tests') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running unit tests"
                            cd build-gcc-Release
                            ctest --output-on-failure --parallel $(nproc)
                        '''
                        publishTestResults testResultsPattern: 'build-gcc-Release/test-results.xml'
                    }
                }
                stage('Integration Tests') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running integration tests"
                            cd build-gcc-Release
                            make integration-tests || true
                            python3 ../scripts/run_integration_tests.py
                        '''
                        publishTestResults testResultsPattern: 'build-gcc-Release/integration-test-results.xml'
                    }
                }
                stage('Performance Tests') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running performance tests"
                            cd build-gcc-Release
                            make performance-tests || true
                            python3 ../scripts/analyze_performance.py
                        '''
                        publishPerformanceResults config: 'performance-config.xml'
                    }
                }
            }
        }
        
        stage('Security') {
            parallel {
                stage('Static Analysis') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running static security analysis"
                            find . -name "*.cpp" -o -name "*.c" | xargs clang-tidy --warnings-as-errors='*' -checks='-*,security*,bugprone-*' || true
                            cppcheck --enable=all --xml --xml-version=2 . 2> cppcheck-report.xml || true
                        '''
                        publishHTML([
                            allowMissing: false,
                            alwaysLinkToLastBuild: true,
                            keepAll: true,
                            reportDir: '.',
                            reportFiles: 'cppcheck-report.xml',
                            reportName: 'Static Analysis Report'
                        ])
                    }
                }
                stage('Dependency Scanning') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running dependency scanning"
                            pip3 install safety bandit
                            safety check --json --output safety-report.json || true
                            bandit -r . -f json -o bandit-report.json || true
                        '''
                        archiveArtifacts artifacts: 'safety-report.json,bandit-report.json', fingerprint: true
                    }
                }
                stage('Container Scanning') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running container security scanning"
                            docker build -t meta-repo-architecture/test .
                            docker run --rm -v /var/run/docker.sock:/var/run/docker.sock aquasec/trivy image --format json --output trivy-report.json meta-repo-architecture/test || true
                        '''
                        archiveArtifacts artifacts: 'trivy-report.json', fingerprint: true
                    }
                }
            }
        }
        
        stage('Performance Analysis') {
            parallel {
                stage('Benchmarking') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running detailed performance benchmarks"
                            cd build-gcc-Release
                            make benchmarks || true
                            python3 ../scripts/run_benchmarks.py
                        '''
                        archiveArtifacts artifacts: 'build-gcc-Release/benchmark-results.json', fingerprint: true
                    }
                }
                stage('Memory Analysis') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Running memory analysis with Valgrind"
                            cd build-gcc-Release
                            valgrind --leak-check=full --error-exitcode=1 --xml=yes --xml-file=valgrind-report.xml ./neuro_fcc_demo || true
                            valgrind --leak-check=full --error-exitcode=1 --xml=yes --xml-file=vortex-shield-valgrind.xml ./vortex_shield_demo || true
                        '''
                        archiveArtifacts artifacts: 'build-gcc-Release/valgrind-report.xml,build-gcc-Release/vortex-shield-valgrind.xml', fingerprint: true
                    }
                }
            }
        }
        
        stage('Documentation') {
            agent {
                label 'linux'
            }
            steps {
                sh '''
                    echo "Generating documentation"
                    chmod +x scripts/generate-docs.sh
                    ./scripts/generate-docs.sh
                '''
                archiveArtifacts artifacts: 'docs/**/*', fingerprint: true
                publishHTML([
                    allowMissing: false,
                    alwaysLinkToLastBuild: true,
                    keepAll: true,
                    reportDir: 'docs/html',
                    reportFiles: 'index.html',
                    reportName: 'Documentation'
                ])
            }
        }
        
        stage('Quality Gate') {
            agent {
                label 'linux'
            }
            steps {
                withSonarQubeEnv('SonarQube') {
                    sh '''
                        echo "Running SonarQube analysis"
                        sonar-scanner \
                            -Dsonar.projectKey=meta-repo-architecture \
                            -Dsonar.sources=. \
                            -Dsonar.host.url=$SONAR_HOST_URL \
                            -Dsonar.login=$SONAR_TOKEN \
                            -Dsonar.coverage.exclusions="**/test/**" \
                            -Dsonar.cpd.exclusions="**/test/**"
                    '''
                }
                timeout(time: 1, unit: 'HOURS') {
                    waitForQualityGate abortPipeline: true
                }
            }
        }
        
        stage('Deploy') {
            when {
                anyOf {
                    branch 'main'
                    branch 'develop'
                    tag pattern: "v\\d+\\.\\d+\\.\\d+", comparator: "REGEXP"
                }
            }
            parallel {
                stage('Docker Deploy') {
                    agent {
                        label 'linux'
                    }
                    steps {
                        sh '''
                            echo "Building and pushing Docker images"
                            for tech in air-to-air-mesh neuro-fcc self-adaptive-rotor-blades cold-jet local-gravity-field-navigation predictive-airflow-engine self-healing-avionics-bios vortex-shield air-swarm-os star-nav-core; do
                                if [ -d "$tech" ]; then
                                    echo "Building Docker image for $tech"
                                    docker build -t $DOCKER_REGISTRY/$tech:$BUILD_NUMBER $tech/
                                    echo $DOCKER_CREDENTIALS_PSW | docker login -u $DOCKER_CREDENTIALS_USR --password-stdin $DOCKER_REGISTRY
                                    docker push $DOCKER_REGISTRY/$tech:$BUILD_NUMBER
                                fi
                            done
                        '''
                    }
                }
                stage('Staging Deploy') {
                    agent {
                        label 'linux'
                    }
                    when {
                        branch 'develop'
                    }
                    steps {
                        sh '''
                            echo "Deploying to staging environment"
                            scp -r build-gcc-Release/* $STAGING_USER@$STAGING_SERVER:/opt/meta-repo-architecture/staging/
                            ssh $STAGING_USER@$STAGING_SERVER "cd /opt/meta-repo-architecture/staging && ./deploy-staging.sh"
                        '''
                    }
                }
                stage('Production Deploy') {
                    agent {
                        label 'linux'
                    }
                    when {
                        allOf {
                            branch 'main'
                            expression { currentBuild.result == null || currentBuild.result == 'SUCCESS' }
                        }
                    }
                    input {
                        message "Deploy to production?"
                        ok "Deploy"
                    }
                    steps {
                        sh '''
                            echo "Deploying to production environment"
                            scp -r build-gcc-Release/* $PRODUCTION_USER@$PRODUCTION_SERVER:/opt/meta-repo-architecture/production/
                            ssh $PRODUCTION_USER@$PRODUCTION_SERVER "cd /opt/meta-repo-architecture/production && ./deploy-production.sh"
                        '''
                    }
                }
            }
        }
        
        stage('Release') {
            when {
                tag pattern: "v\\d+\\.\\d+\\.\\d+", comparator: "REGEXP"
            }
            agent {
                label 'linux'
            }
            steps {
                sh '''
                    echo "Creating release"
                    VERSION=$(git describe --tags --abbrev=0)
                    DESCRIPTION="Meta-repo-architecture $VERSION - 10 Revolutionary Aviation Technologies"
                    
                    # Create release assets
                    mkdir -p release-assets
                    for tech in air-to-air-mesh neuro-fcc self-adaptive-rotor-blades cold-jet local-gravity-field-navigation predictive-airflow-engine self-healing-avionics-bios vortex-shield air-swarm-os star-nav-core; do
                        if [ -d "$tech" ]; then
                            tar -czf release-assets/$tech-$VERSION.tar.gz $tech/
                        fi
                    done
                    
                    # Create GitHub release
                    curl -X POST \
                        -H "Authorization: token $GITHUB_TOKEN" \
                        -H "Accept: application/vnd.github.v3+json" \
                        https://api.github.com/repos/$JOB_NAME/releases \
                        -d "{
                            \"tag_name\": \"$VERSION\",
                            \"target_commitish\": \"$GIT_COMMIT\",
                            \"name\": \"$VERSION\",
                            \"body\": \"$DESCRIPTION\",
                            \"draft\": false,
                            \"prerelease\": false
                        }"
                '''
                archiveArtifacts artifacts: 'release-assets/**/*', fingerprint: true
            }
        }
    }
    
    post {
        always {
            cleanWs()
        }
        success {
            script {
                if (env.SLACK_WEBHOOK) {
                    slackSend(
                        channel: '#ci-cd',
                        color: 'good',
                        message: "✅ Pipeline succeeded for ${env.JOB_NAME} (${env.BRANCH_NAME})"
                    )
                }
            }
        }
        failure {
            script {
                if (env.SLACK_WEBHOOK) {
                    slackSend(
                        channel: '#ci-cd',
                        color: 'danger',
                        message: "❌ Pipeline failed for ${env.JOB_NAME} (${env.BRANCH_NAME}). Check ${env.BUILD_URL}"
                    )
                }
            }
        }
        unstable {
            script {
                if (env.SLACK_WEBHOOK) {
                    slackSend(
                        channel: '#ci-cd',
                        color: 'warning',
                        message: "⚠️ Pipeline unstable for ${env.JOB_NAME} (${env.BRANCH_NAME})"
                    )
                }
            }
        }
        aborted {
            script {
                if (env.SLACK_WEBHOOK) {
                    slackSend(
                        channel: '#ci-cd',
                        color: 'warning',
                        message: "⏹️ Pipeline aborted for ${env.JOB_NAME} (${env.BRANCH_NAME})"
                    )
                }
            }
        }
    }
}
