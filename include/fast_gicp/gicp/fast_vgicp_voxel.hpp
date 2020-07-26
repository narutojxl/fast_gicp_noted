#ifndef FAST_GICP_FAST_VGICP_VOXEL_HPP
#define FAST_GICP_FAST_VGICP_VOXEL_HPP

namespace fast_gicp {

class Vector3iHash {
public:
  size_t operator()(const Eigen::Vector3i& x) const {
    size_t seed = 0;
    boost::hash_combine(seed, x[0]); //用x，y，z三个分量计算hash值，作为map, unordered_map的key
    boost::hash_combine(seed, x[1]);
    boost::hash_combine(seed, x[2]);
    return seed;
  }
};



struct GaussianVoxel {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  using Ptr = std::shared_ptr<GaussianVoxel>;

  GaussianVoxel() {
    num_points = 0;
    mean.setZero();
    cov.setZero();
  }
  virtual ~GaussianVoxel() {}

  virtual void append(const Eigen::Vector4f& mean_, const Eigen::Matrix4f& cov_) = 0; //纯虚函数

  virtual void finalize() = 0;

public:
  int num_points;
  Eigen::Vector4f mean;
  Eigen::Matrix4f cov;
};



struct MultiplicativeGaussianVoxel : GaussianVoxel {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  MultiplicativeGaussianVoxel() : GaussianVoxel() {}
  virtual ~MultiplicativeGaussianVoxel() {}

  virtual void append(const Eigen::Vector4f& mean_, const Eigen::Matrix4f& cov_) override {
    num_points++;
    Eigen::Matrix4f cov_inv = cov_;
    cov_inv(3, 3) = 1;
    cov_inv = cov_inv.inverse().eval();

    cov += cov_inv;
    mean += cov_inv * mean_; //均值的更新公式和下面的不一样。
  }

  virtual void finalize() override {
    cov(3, 3) = 1;
    mean[3] = 1;

    cov = cov.inverse().eval();
    mean = (cov * mean).eval();
  }
};



struct AdditiveGaussianVoxel : GaussianVoxel {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  AdditiveGaussianVoxel() : GaussianVoxel() {}
  virtual ~AdditiveGaussianVoxel() {}

  virtual void append(const Eigen::Vector4f& mean_, const Eigen::Matrix4f& cov_) override {
    num_points++;
    mean += mean_; //paper 伪代码23～25
    cov += cov_;
  }

  virtual void finalize() override {
    mean /= num_points; //paper 伪代码26～28
    cov /= num_points;
  }
};

}  // namespace fast_gicp

#endif