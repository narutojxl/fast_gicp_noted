#ifndef FAST_GICP_FAST_GICP_ST_HPP
#define FAST_GICP_FAST_GICP_ST_HPP

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/registration/registration.h>
#include <fast_gicp/gicp/gicp_settings.hpp>

namespace fast_gicp {

/**
 * @brief Fast GICP algorithm optimized for single threading  
 *  
 *   作者对GICP的实现
 * 
 */
template<typename PointSource, typename PointTarget>
class FastGICPSingleThread : public pcl::Registration<PointSource, PointTarget, float> {
public:
  using Scalar = float;
  using Matrix4 = typename pcl::Registration<PointSource, PointTarget, Scalar>::Matrix4;

  using PointCloudSource = typename pcl::Registration<PointSource, PointTarget, Scalar>::PointCloudSource;
  using PointCloudSourcePtr = typename PointCloudSource::Ptr;
  using PointCloudSourceConstPtr = typename PointCloudSource::ConstPtr;

  using PointCloudTarget = typename pcl::Registration<PointSource, PointTarget, Scalar>::PointCloudTarget;
  using PointCloudTargetPtr = typename PointCloudTarget::Ptr;
  using PointCloudTargetConstPtr = typename PointCloudTarget::ConstPtr;

  using pcl::Registration<PointSource, PointTarget, Scalar>::reg_name_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::input_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::target_;

  using pcl::Registration<PointSource, PointTarget, Scalar>::tree_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::tree_reciprocal_;

  using pcl::Registration<PointSource, PointTarget, Scalar>::nr_iterations_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::max_iterations_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::final_transformation_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::transformation_epsilon_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::converged_;
  using pcl::Registration<PointSource, PointTarget, Scalar>::corr_dist_threshold_;

  FastGICPSingleThread();
  virtual ~FastGICPSingleThread() override;

  void setRotationEpsilon(double eps);

  void setCorrespondenceRandomness(int k);

  void setRegularizationMethod(RegularizationMethod method);

  void swapSourceAndTarget();

  void clearSource();

  void clearTarget();

  virtual void setInputSource(const PointCloudSourceConstPtr& cloud) override;

  virtual void setInputTarget(const PointCloudTargetConstPtr& cloud) override;

protected:
  virtual void computeTransformation(PointCloudSource& output, const Matrix4& guess) override;

private:
  bool is_converged(const Eigen::Matrix<float, 6, 1>& delta) const;

  void update_correspondences(const Eigen::Matrix<float, 6, 1>& x);

  Eigen::VectorXf loss_ls(const Eigen::Matrix<float, 6, 1>& x, Eigen::MatrixXf* J) const;

  template<typename PointT>
  bool calculate_covariances(const boost::shared_ptr<const pcl::PointCloud<PointT>>& cloud, pcl::search::KdTree<PointT>& kdtree, std::vector<Matrix4, Eigen::aligned_allocator<Matrix4>>& covariances);

private:
  double rotation_epsilon_;
  int k_correspondences_;
  RegularizationMethod regularization_method_;

  std::unique_ptr<pcl::search::KdTree<PointSource>> source_kdtree;
  std::unique_ptr<pcl::search::KdTree<PointTarget>> target_kdtree;

  std::vector<Matrix4, Eigen::aligned_allocator<Matrix4>> source_covs;
  std::vector<Matrix4, Eigen::aligned_allocator<Matrix4>> target_covs;

  std::vector<int> correspondences;
  std::vector<float> sq_distances;

  std::vector<float> second_sq_distances;
  std::vector<Eigen::Vector4f, Eigen::aligned_allocator<Eigen::Vector4f>> anchors;
};
}  // namespace fast_gicp

#endif
