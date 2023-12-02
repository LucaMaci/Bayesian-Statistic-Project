//
// Created by Valeria Montefusco on 30/11/23.
//

#include "auxiliar_functions.h"




matrix_collection GibbsSampler_IBP(const double alpha,const double gamma,const double sigma_a, const double sigma_x, const double theta, const int n, MatrixXd &A,MatrixXd &X, unsigned n_iter,unsigned initial_iters){
    std::default_random_engine generator;

    /* In this algorithm matrix Z are generated keeping only non null columns,
     * this makes the algorithm to go really slow beacuse the complexity icreases.
     * WARNING: use gamma small beacuse otherwise the algorithm will be way slower
     * */

    // Initialization of Z and m
    MatrixXd Z(n,1);
    VectorXd m;

    // D:
    const unsigned D=A.cols();

    //create a vector to put the generated Z matrices:
    matrix_collection Ret;

    for (size_t it=0;it<n_iter+initial_iters;++it){

        MatrixXd Znew;

        for (size_t i=0; i<n;++i){
            size_t z_cols=Z.cols();

            MatrixXd M(z_cols,z_cols);

            M=(Z.transpose()*Z -  Eigen::MatrixXd::Identity(z_cols,z_cols)*pow(sigma_x/sigma_a,2)).inverse();

            Eigen::VectorXd z_i=Z.row(i);

            Z.row(i).setZero();

            VectorXd positions;
            auto matvec=std::make_pair(Znew,positions);
            matvec= eliminate_null_columns(Z);
            Znew=matvec.first;
            positions=matvec.second;

            Z.row(i)=z_i;
            m=fill_m(Znew);

            //update the number of observed features:
            unsigned  K= count_nonzero(m);

            size_t count=0;

            for(size_t j=0;j<K; ++j) {
                while(positions(count)==0)
                    ++count;

                double prob_zz=(m(j)-alpha)/(theta+(n-1));

                //P(X|Z)

                Z(i,count)=1;
                M= update_M(M,Z.row(i));
                /*double prob_xz_1=1/(pow(2*M_PI,n*D*0.5)* pow(sigma_x,(n-K)*D)*
                                    pow(sigma_a,K*D)*
                                    pow((Z.transpose()*Z + sigma_x*sigma_x/sigma_a/sigma_a*Eigen::MatrixXd::Identity(n_tilde,n_tilde)).determinant(),D*0.5) );
                                    */

                long double prob_xz_1=1/(pow((Z.transpose()*Z + sigma_x*sigma_x/sigma_a/sigma_a*Eigen::MatrixXd::Identity(z_cols,z_cols)).determinant(),D*0.5) );


                MatrixXd mat=X.transpose() * (Eigen::MatrixXd::Identity(n,n) - (Z * M * Z.transpose())) * X;
                long double prob_xz_2=mat.trace()*(-1/(2*sigma_x*sigma_x));

                long double prob_xz=prob_xz_1*exp(prob_xz_2);

                Z(i,count)=0;
                M= update_M(M,Z.row(i));
                long double prob_xz_10=1/pow((Z.transpose()*Z + sigma_x*sigma_x/sigma_a/sigma_a*Eigen::MatrixXd::Identity(Z.cols(),Z.cols())).determinant(),D*0.5);

                MatrixXd mat0=X.transpose() * (Eigen::MatrixXd::Identity(n,n) - (Z * M * Z.transpose())) * X;
                long double prob_xz_20=mat.trace()*(-1/(2*sigma_x*sigma_x));

                long double prob_xz0=prob_xz_10*exp(prob_xz_20);

                long double prob_one_temp=prob_zz*prob_xz;
                long double prob_zero_temp=(1-prob_zz)*prob_xz0;
                long double prob_param=prob_one_temp/(prob_one_temp+prob_zero_temp); //PROBLEM: always too small
                //  std::cout << prob_param <<std::endl;



                //sample from Bernoulli distribution:

                std::bernoulli_distribution distribution_bern(prob_param);

                Znew(i,j) = distribution_bern(generator) ? 1:0;
                Z(i,count)=Znew(i,j);



                //  std::cout << "Matrix M:\n" << M << "\n";

                ++count;
            }


            //sample the number of new features:
            double param=tgamma(theta+alpha+n)/tgamma(theta+alpha)*tgamma(theta)/tgamma(theta+n)*gamma;
            std::poisson_distribution<int> distribution_poi(param);
            int temp = distribution_poi(generator);

            //update Z:
            Z.resize(n,K+temp);
            for(size_t j=0;j<K;++j)
                Z.col(j)=Znew.col(j);
            for(size_t j=K;j<K+temp;++j)
            {
                Z.col(j).setZero();
                Z(i,j)=1;
            }

        }


        if(it>=initial_iters){

            Ret.push_back(Z);
        }

    }
    return Ret;
}