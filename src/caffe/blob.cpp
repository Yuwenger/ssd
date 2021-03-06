#include <climits>
#include <vector>

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/syncedmem.hpp"
#include "caffe/caffe.hpp"
#include "caffe/util/math_functions.hpp"
namespace caffe {


template <typename Dtype>
void Blob<Dtype>::Reshape(const int num, const int channels, const int height,
                          const int width) {
	vector<int> shape(4);
	shape[0] = num;
	shape[1] = channels;
	shape[2] = height;
	shape[3] = width;
	Reshape(shape);
}

template <typename Dtype>
void Blob<Dtype>::Reshape(const vector<int>& shape) {
	CHECK_LE(shape.size(), kMaxBlobAxes);
	count_ = 1;
	nnz_=1;
	shape_.resize(shape.size());
	if (!shape_data_ || shape_data_->size() < shape.size() * sizeof(int)) {
		shape_data_.reset(new SyncedMemory(shape.size() * sizeof(int)));
	}
	int* shape_data = static_cast<int*>(shape_data_->mutable_cpu_data());
	for (int i = 0; i < shape.size(); ++i) {
		CHECK_GE(shape[i], 0);
		if (count_ != 0) {
			CHECK_LE(shape[i], INT_MAX / count_) << "blob size exceeds INT_MAX";
		}
		count_ *= shape[i];
		shape_[i] = shape[i];
		shape_data[i] = shape[i];
	}
	if (count_ > capacity_) {
		capacity_ = count_;
		data_.reset(new SyncedMemory(capacity_ * sizeof(Dtype)));
		diff_.reset(new SyncedMemory(capacity_ * sizeof(Dtype)));
		if(sparse_&&(FLAGS_step!="three"))
			mask_.reset(new SyncedMemory(capacity_ * sizeof(Dtype)));

	}
}

template <typename Dtype>
void Blob<Dtype>::Addmask(const vector<int>& shape) {
	if(count_>=capacity_) {
		capacity_=count_;
//    if(sparse_&&(FLAGS_step!="three"))
		mask_.reset(new SyncedMemory(capacity_*sizeof(Dtype)));
		Dtype* mask=mutable_cpu_mask();
		for(int i=0; i<count_; i++)
			mask[i]=1;
	}
}


template <typename Dtype>
void Blob<Dtype>::Reshape(const BlobShape& shape) {
	CHECK_LE(shape.dim_size(), kMaxBlobAxes);
	vector<int> shape_vec(shape.dim_size());
	for (int i = 0; i < shape.dim_size(); ++i) {
		shape_vec[i] = shape.dim(i);
	}
	Reshape(shape_vec);
}

template <typename Dtype>
void Blob<Dtype>::ReshapeLike(const Blob<Dtype>& other) {
	Reshape(other.shape());
}

template <typename Dtype>
Blob<Dtype>::Blob(const int num, const int channels, const int height,
                  const int width)
// capacity_ must be initialized before calling Reshape
	: capacity_(0) {
	if(!sparse_||FLAGS_step!="three")
		Reshape(num, channels, height, width);
}

template <typename Dtype>
Blob<Dtype>::Blob(const vector<int>& shape)
// capacity_ must be initialized before calling Reshape
	: capacity_(0),sparse_(false) {
	if(!sparse_||FLAGS_step!="three") {
		Reshape(shape);
	}
}

template <typename Dtype>
const int* Blob<Dtype>::gpu_shape() const {
	CHECK(shape_data_);
	return (const int*)shape_data_->gpu_data();
}

template <typename Dtype>
const Dtype* Blob<Dtype>::cpu_data() const {
	CHECK(data_);
	return (const Dtype*)data_->cpu_data();
}

template <typename Dtype>
void Blob<Dtype>::set_cpu_data(Dtype* data) {
	CHECK(data);
	data_->set_cpu_data(data);
}

template <typename Dtype>
const Dtype* Blob<Dtype>::gpu_data() const {
	CHECK(data_);
	return (const Dtype*)data_->gpu_data();
}

template <typename Dtype>
const Dtype* Blob<Dtype>::cpu_diff() const {
	CHECK(diff_);
	return (const Dtype*)diff_->cpu_data();
}

template <typename Dtype>
const Dtype* Blob<Dtype>::gpu_diff() const {
	CHECK(diff_);
	return (const Dtype*)diff_->gpu_data();
}

template <typename Dtype>
const Dtype* Blob<Dtype>::cpu_mask() const {
	CHECK(mask_);
	return (const Dtype*)mask_->cpu_data();
}
template <typename Dtype>
const Dtype* Blob<Dtype>::gpu_mask()const {
	CHECK(mask_);
	return (const Dtype*)mask_->gpu_data();
}
template <typename Dtype>
const Dtype* Blob<Dtype>::cpu_csrval() const {
	CHECK(csrval_);
	return (const Dtype*)csrval_->cpu_data();
}
template <typename Dtype>
const Dtype* Blob<Dtype>::gpu_csrval()const {
	CHECK(csrval_);
	return (const Dtype*)csrval_->gpu_data();
}
template <typename Dtype>
const int* Blob<Dtype>::cpu_csrrowptr() const {
	CHECK(csrrowptr_);
	return (const int*)csrrowptr_->cpu_data();
}
template <typename Dtype>
const int* Blob<Dtype>::gpu_csrrowptr()const {
	CHECK(csrrowptr_);
	return (const int*)csrrowptr_->gpu_data();
}
template <typename Dtype>
const int* Blob<Dtype>::cpu_csrcolind() const {
	CHECK(csrcolind_);
	return (const int*)csrcolind_->cpu_data();
}
template <typename Dtype>
const int* Blob<Dtype>::gpu_csrcolind()const {
	CHECK(csrcolind_);
	return (const int*)csrcolind_->gpu_data();
}
template <typename Dtype>
Dtype* Blob<Dtype>::mutable_cpu_data() {
	CHECK(data_);
	return static_cast<Dtype*>(data_->mutable_cpu_data());
}

template <typename Dtype>
Dtype* Blob<Dtype>::mutable_gpu_data() {
	CHECK(data_);
	return static_cast<Dtype*>(data_->mutable_gpu_data());
}

template <typename Dtype>
Dtype* Blob<Dtype>::mutable_cpu_diff() {
	CHECK(diff_);
	return static_cast<Dtype*>(diff_->mutable_cpu_data());
}

template <typename Dtype>
Dtype* Blob<Dtype>::mutable_gpu_diff() {
	CHECK(diff_);
	return static_cast<Dtype*>(diff_->mutable_gpu_data());
}

template <typename Dtype>
Dtype* Blob<Dtype>::mutable_cpu_mask() {
	CHECK(mask_);
	return static_cast<Dtype*>(mask_->mutable_cpu_data());
}

template <typename Dtype>
Dtype* Blob<Dtype>::mutable_gpu_mask() {
	CHECK(mask_);
	return static_cast<Dtype*>(mask_->mutable_gpu_data());
}
template <typename Dtype>
Dtype* Blob<Dtype>::mutable_cpu_csrval() {
	CHECK(csrval_);
	return static_cast<Dtype*>(csrval_->mutable_cpu_data());
}

template <typename Dtype>
int* Blob<Dtype>::mutable_gpu_csrrowptr() {
	CHECK(csrrowptr_);
	return static_cast<int*>(csrrowptr_->mutable_gpu_data());
}

template <typename Dtype>
int* Blob<Dtype>::mutable_cpu_csrrowptr() {
	CHECK(csrrowptr_);
	return static_cast<int*>(csrrowptr_->mutable_cpu_data());
}
template <typename Dtype>
int* Blob<Dtype>::mutable_gpu_csrcolind() {
	CHECK(csrcolind_);
	return static_cast<int*>(csrcolind_->mutable_gpu_data());
}
template <typename Dtype>
int* Blob<Dtype>::mutable_cpu_csrcolind() {
	CHECK(csrcolind_);
	return static_cast<int*>(csrcolind_->mutable_cpu_data());
}

template <typename Dtype>
Dtype* Blob<Dtype>::mutable_gpu_csrval() {
	CHECK(csrval_);
	return static_cast<Dtype*>(csrval_->mutable_gpu_data());
}

template <typename Dtype>
void Blob<Dtype>::ShareData(const Blob& other) {
	CHECK_EQ(count_, other.count());
	data_ = other.data();
}

template <typename Dtype>
void Blob<Dtype>::ShareDiff(const Blob& other) {
	CHECK_EQ(count_, other.count());
	diff_ = other.diff();
}

template <typename Dtype>
void Blob<Dtype>::ShareMask(const Blob& other) {
	CHECK_EQ(count_, other.count());
	mask_ = other.mask();
}
//template <typename Dtype>
//void Blob<Dtype>::ShareCsrval(const Blob& other) {
//  CHECK_EQ(nnz_, other.nnz());
//  csrval_ = other.csrval();
//}
// The "tpdate" method is used for parameter blobs in a Net, which are stored
// as Blob<float> or Blob<double> -- hence we do not define it for
// Blob<int> or Blob<unsigned int>.
template <> void Blob<unsigned int>::Update() {
	NOT_IMPLEMENTED;
}
template <> void Blob<int>::Update() {
	NOT_IMPLEMENTED;
}

template <typename Dtype>
void Blob<Dtype>::Update() {
	// We will perform update based on where the data is located.
	switch (data_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		// perform computation on CPU
		caffe_axpy<Dtype>(count_, Dtype(-1),
		                  static_cast<const Dtype*>(diff_->cpu_data()),
		                  static_cast<Dtype*>(data_->mutable_cpu_data()));
		if(sparse_&&FLAGS_step!="three")
			caffe_mul<Dtype>(count_,
			                 static_cast<const Dtype*>(mask_->cpu_data()),
			                 static_cast<const Dtype*>(data_->cpu_data()),
			                 static_cast<Dtype*>(data_->mutable_cpu_data()));
		break;
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		// perform computation on GPU
		caffe_gpu_axpy<Dtype>(count_, Dtype(-1),
		                      static_cast<const Dtype*>(diff_->gpu_data()),
		                      static_cast<Dtype*>(data_->mutable_gpu_data()));
		// for(int c_idx=0;c_idx<count_;c_idx++)
		//   std::cout<<cpu_mask()[c_idx]<<" ";
		// std::cout<<std::endl;
		if(sparse_&&FLAGS_step!="three")
			caffe_gpu_mul<Dtype>(count_,
			                     static_cast<const Dtype*>(mask_->gpu_data()),
			                     static_cast<const Dtype*>(data_->gpu_data()),
			                     static_cast<Dtype*>(data_->mutable_gpu_data()));
#else
		NO_GPU;
#endif
		break;
	default:
		LOG(FATAL) << "Syncedmem not initialized.";
	}
}

template <> unsigned int Blob<unsigned int>::asum_data() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <> int Blob<int>::asum_data() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <typename Dtype>
Dtype Blob<Dtype>::asum_data() const {
	if (!data_) {
		return 0;
	}
	switch (data_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		return caffe_cpu_asum(count_, cpu_data());
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		{
			Dtype asum;
			caffe_gpu_asum(count_, gpu_data(), &asum);
			return asum;
		}
#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return 0;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();
	}
	return 0;
}

template <> unsigned int Blob<unsigned int>::asum_diff() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <> int Blob<int>::asum_diff() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <typename Dtype>
Dtype Blob<Dtype>::asum_diff() const {
	if (!diff_) {
		return 0;
	}
	switch (diff_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		return caffe_cpu_asum(count_, cpu_diff());
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		{
			Dtype asum;
			caffe_gpu_asum(count_, gpu_diff(), &asum);
			return asum;
		}
#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return 0;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << diff_->head();
	}
	return 0;
}
template <> unsigned int Blob<unsigned int>::asum_mask() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <> int Blob<int>::asum_mask() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <typename Dtype>
Dtype Blob<Dtype>::asum_mask() const {
	if (!mask_) {
		return 0;
	}
	switch (mask_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		return caffe_cpu_asum(count_, cpu_mask());
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		{
			Dtype asum;
			caffe_gpu_asum(count_, gpu_mask(), &asum);
			return asum;
		}
#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return 0;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << mask_->head();
	}
	return 0;
}

template <> unsigned int Blob<unsigned int>::sumsq_data() const {
	NOT_IMPLEMENTED;
	return 0;
}
//template <typename Dtype>
//Dtype Blob<Dtype>::asum_csrval() const {
//  if (!csrval_) { return 0; }
//  switch (csrval_->head()) {
//  case SyncedMemory::HEAD_AT_CPU:
//    return caffe_cpu_asum(nnz_, cpu_csrval());
//  case SyncedMemory::HEAD_AT_GPU:
//  case SyncedMemory::SYNCED:
//#ifndef CPU_ONLY
//  {
//    Dtype asum;
//    caffe_gpu_asum(nnz_, gpu_csrval(), &asum);
//    return asum;
//  }
//#else
//    NO_GPU;
//#endif
//  case SyncedMemory::UNINITIALIZED:
//    return 0;
//  default:
//    LOG(FATAL) << "Unknown SyncedMemory head state: " << csrval_->head();
//  }
//  return 0;
//}

template <> int Blob<int>::sumsq_data() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <typename Dtype>
Dtype Blob<Dtype>::sumsq_data() const {
	Dtype sumsq;
	const Dtype* data;
	if (!data_) {
		return 0;
	}
	switch (data_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		data = cpu_data();
		sumsq = caffe_cpu_dot(count_, data, data);
		break;
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		data = gpu_data();
		caffe_gpu_dot(count_, data, data, &sumsq);
#else
		NO_GPU;
#endif
		break;
	case SyncedMemory::UNINITIALIZED:
		return 0;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();
	}
	return sumsq;
}

template <> unsigned int Blob<unsigned int>::sumsq_diff() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <> int Blob<int>::sumsq_diff() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <typename Dtype>
Dtype Blob<Dtype>::sumsq_diff() const {
	Dtype sumsq;
	const Dtype* diff;
	if (!diff_) {
		return 0;
	}
	switch (diff_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		diff = cpu_diff();
		sumsq = caffe_cpu_dot(count_, diff, diff);
		break;
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		diff = gpu_diff();
		caffe_gpu_dot(count_, diff, diff, &sumsq);
		break;
#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return 0;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();
	}
	return sumsq;
}
template <> unsigned int Blob<unsigned int>::sumsq_mask() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <> int Blob<int>::sumsq_mask() const {
	NOT_IMPLEMENTED;
	return 0;
}

template <typename Dtype>
Dtype Blob<Dtype>::sumsq_mask() const {
	Dtype sumsq;
	const Dtype* mask;
	if (!mask_) {
		return 0;
	}
	switch (mask_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		mask = cpu_mask();
		sumsq = caffe_cpu_dot(count_, mask, mask);
		break;
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		mask = gpu_mask();
		caffe_gpu_dot(count_, mask, mask, &sumsq);
		break;

#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return 0;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << mask_->head();
	}
	return sumsq;
}
//template <typename Dtype>
//Dtype Blob<Dtype>::sumsq_csrval() const {
//  Dtype sumsq;
//  const Dtype* csrval;
//  if (!csrval_) { return 0; }
//  switch (csrval_->head()) {
//  case SyncedMemory::HEAD_AT_CPU:
//    csrval = cpu_csrval();
//    sumsq = caffe_cpu_dot(nnz_, csrval, csrval);
//    break;
//  case SyncedMemory::HEAD_AT_GPU:
//  case SyncedMemory::SYNCED:
//#ifndef CPU_ONLY
//    csrval = gpu_csrval();
//    caffe_gpu_dot(nnz_, csrval, csrval, &sumsq);
//    break;
//
//#else
//    NO_GPU;
//#endif
//  case SyncedMemory::UNINITIALIZED:
//    return 0;
//  default:
//    LOG(FATAL) << "Unknown SyncedMemory head state: " << csrval_->head();
//  }
//  return sumsq;
//}

template <> void Blob<unsigned int>::scale_data(unsigned int scale_factor) {
	NOT_IMPLEMENTED;
}

template <> void Blob<int>::scale_data(int scale_factor) {
	NOT_IMPLEMENTED;
}

template <typename Dtype>
void Blob<Dtype>::scale_data(Dtype scale_factor) {
	Dtype* data;
	if (!data_) {
		return;
	}
	switch (data_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		data = mutable_cpu_data();
		caffe_scal(count_, scale_factor, data);
		return;
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		data = mutable_gpu_data();
		caffe_gpu_scal(count_, scale_factor, data);
		return;
#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();
	}
}

template <> void Blob<unsigned int>::scale_diff(unsigned int scale_factor) {
	NOT_IMPLEMENTED;
}

template <> void Blob<int>::scale_diff(int scale_factor) {
	NOT_IMPLEMENTED;
}

template <typename Dtype>
void Blob<Dtype>::scale_diff(Dtype scale_factor) {
	Dtype* diff;
	if (!diff_) {
		return;
	}
	switch (diff_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		diff = mutable_cpu_diff();
		caffe_scal(count_, scale_factor, diff);
		return;
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		diff = mutable_gpu_diff();
		caffe_gpu_scal(count_, scale_factor, diff);
		return;
#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << diff_->head();
	}
}
template <> void Blob<unsigned int>::scale_mask(unsigned int scale_factor) {
	NOT_IMPLEMENTED;
}

template <> void Blob<int>::scale_mask(int scale_factor) {
	NOT_IMPLEMENTED;
}

template <typename Dtype>
void Blob<Dtype>::scale_mask(Dtype scale_factor) {
	Dtype* mask;
	if (!mask_) {
		return;
	}
	switch (mask_->head()) {
	case SyncedMemory::HEAD_AT_CPU:
		mask = mutable_cpu_mask();
		caffe_scal(count_, scale_factor, mask);
		return;
	case SyncedMemory::HEAD_AT_GPU:
	case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
		mask = mutable_gpu_mask();
		caffe_gpu_scal(count_, scale_factor, mask);
		return;
#else
		NO_GPU;
#endif
	case SyncedMemory::UNINITIALIZED:
		return;
	default:
		LOG(FATAL) << "Unknown SyncedMemory head state: " << mask_->head();
	}
}

template <typename Dtype>
bool Blob<Dtype>::ShapeEquals(const BlobProto& other) {
	if (other.has_num() || other.has_channels() ||
	        other.has_height() || other.has_width()) {
		// Using deprecated 4D Blob dimensions --
		// shape is (num, channels, height, width).
		// Note: we do not use the normal Blob::num(), Blob::channels(), etc.
		// methods as these index from the beginning of the blob shape, where legacy
		// parameter blobs were indexed from the end of the blob shape (e.g., bias
		// Blob shape (1 x 1 x 1 x N), IP layer weight Blob shape (1 x 1 x M x N)).
		return shape_.size() <= 4 &&
		       LegacyShape(-4) == other.num() &&
		       LegacyShape(-3) == other.channels() &&
		       LegacyShape(-2) == other.height() &&
		       LegacyShape(-1) == other.width();
	}
	vector<int> other_shape(other.shape().dim_size());
	for (int i = 0; i < other.shape().dim_size(); ++i) {
		other_shape[i] = other.shape().dim(i);
	}
	return shape_ == other_shape;
}

template <typename Dtype>
void Blob<Dtype>::CopyFrom(const Blob& source, bool copy_diff, bool reshape) {
	if (source.count() != count_ || source.shape() != shape_) {
		if (reshape) {
			ReshapeLike(source);
		} else {
			LOG(FATAL) << "Trying to copy blobs of different sizes.";
		}
	}
	LOG(INFO)<<"copying from";
	switch (Caffe::mode()) {
	case Caffe::GPU:
		if (copy_diff) {
			caffe_copy(count_, source.gpu_diff(),
			           static_cast<Dtype*>(diff_->mutable_gpu_data()));
		} else {
			caffe_copy(count_, source.gpu_data(),
			           static_cast<Dtype*>(data_->mutable_gpu_data()));
			caffe_copy(count_,source.gpu_mask(),
			           static_cast<Dtype*>(mask_->mutable_gpu_data()));
		}
		break;
	case Caffe::CPU:
		if (copy_diff) {
			caffe_copy(count_, source.cpu_diff(),
			           static_cast<Dtype*>(diff_->mutable_cpu_data()));
		} else {
			caffe_copy(count_, source.cpu_data(),
			           static_cast<Dtype*>(data_->mutable_cpu_data()));
			caffe_copy(count_,source.cpu_mask(),
			           static_cast<Dtype*>(mask_->mutable_cpu_data()));
		}
		break;
	default:
		LOG(FATAL) << "Unknown caffe mode.";
	}
}

template <typename Dtype>
void Blob<Dtype>::FromProto(const BlobProto& proto, bool reshape) {
	if(FLAGS_step!="three"||!proto.sparse()) {
		if (reshape) {
			vector<int> shape;
			if (proto.has_num() || proto.has_channels() ||
			        proto.has_height() || proto.has_width()) {
				// Using deprecated 4D Blob dimensions --
				// shape is (num, channels, height, width).
				shape.resize(4);
				shape[0] = proto.num();
				shape[1] = proto.channels();
				shape[2] = proto.height();
				shape[3] = proto.width();
			} else {
				shape.resize(proto.shape().dim_size());
				for (int i = 0; i < proto.shape().dim_size(); ++i) {
					shape[i] = proto.shape().dim(i);
				}
			}
			Reshape(shape);
		} else {
			CHECK(ShapeEquals(proto)) << "shape mismatch (reshape not set)";
		}
		// copy data
		if (proto.double_data_size() > 0) {
			CHECK_EQ(count_, proto.double_data_size());
			Dtype* data_vec = mutable_cpu_data();
			for (int i = 0; i < count_; ++i) {
				data_vec[i] = proto.double_data(i);
			}
		} else if(proto.data_size()>0) {
			CHECK_EQ(count_, proto.data_size());
			Dtype* data_vec = mutable_cpu_data();
			for (int i = 0; i < count_; ++i) {
				data_vec[i] = proto.data(i);
			}
		}
		if (proto.double_diff_size() > 0) {
			CHECK_EQ(count_, proto.double_diff_size());
			Dtype* diff_vec = mutable_cpu_diff();
			for (int i = 0; i < count_; ++i) {
				diff_vec[i] = proto.double_diff(i);
			}
		} else if (proto.diff_size() > 0) {
			CHECK_EQ(count_, proto.diff_size());
			Dtype* diff_vec = mutable_cpu_diff();
			for (int i = 0; i < count_; ++i) {
				diff_vec[i] = proto.diff(i);
			}
		}
		if(sparse_&&FLAGS_step!="three") {
			if (proto.double_mask_size() > 0) {
				CHECK_EQ(count_, proto.double_mask_size());
				Dtype* mask_vec = mutable_cpu_mask();
				for (int i = 0; i < count_; ++i) {
					mask_vec[i] = proto.double_mask(i);
				}
			} else if (proto.mask_size() > 0) {
				CHECK_EQ(count_, proto.mask_size());
				Dtype* mask_vec = mutable_cpu_mask();
				for (int i = 0; i < count_; ++i) {
					mask_vec[i] = proto.mask(i);
				}
			}
		}
	}

	if(proto.csrval_size()>0) {
		CHECK_EQ(proto.nnz(),proto.csrval_size());
		csrval_.reset(new SyncedMemory(proto.nnz()*sizeof(Dtype)));
		Dtype* csrval_vec=mutable_cpu_csrval();
		setNnz(proto.nnz());
		LOG(INFO)<<"csrval_size()>0"<<" "<<nnz();
		for(int i=0; i<proto.nnz(); i++) {
			csrval_vec[i]=proto.csrval(i);
		}
	} else if(proto.double_csrval_size()>0) {
		CHECK_EQ(proto.nnz(),proto.double_csrval_size());
		csrval_.reset(new SyncedMemory(proto.nnz()*sizeof(Dtype)));
		Dtype* csrval_vec=mutable_cpu_csrval();
		setNnz(proto.nnz());
		for(int i=0; i<nnz_; i++) {
			csrval_vec[i]=proto.double_csrval(i);
		}
	}
	if(proto.csrrowptr_size()>0) {
		CHECK_EQ(proto.csrrowptr_size(),shape_[0]+1);
		csrrowptr_.reset(new SyncedMemory(sizeof(int)*(shape_[0]+1)));
		int* csrrowptr_vec=mutable_cpu_csrrowptr();
		LOG(INFO)<<"csrrowptr_size>0";
		for(int i=0; i<shape_[0]+1; i++) {
			csrrowptr_vec[i]=proto.csrrowptr(i);
		}
	}
	if(proto.csrcolind_size()>0) {
		CHECK_EQ(proto.csrcolind_size(),proto.nnz());
		csrcolind_.reset(new SyncedMemory(sizeof(int)*nnz_));
		int* csrcolind_vec=mutable_cpu_csrcolind();
		LOG(INFO)<<"csrcolind_size>0";
		for(int i=0; i<nnz_; i++) {
			csrcolind_vec[i]=proto.csrcolind(i);
		}
	}
}

template <>
void Blob<double>::ToProto(BlobProto* proto, bool write_diff)  {
	proto->clear_shape();
	for (int i = 0; i < shape_.size(); ++i) {
		proto->mutable_shape()->add_dim(shape_[i]);
	}
	proto->clear_double_data();
	proto->clear_double_diff();
	proto->clear_double_mask();
	proto->clear_double_csrval();
	const double* data_vec = cpu_data();
	const double* mask_vec = cpu_mask();
	for (int i = 0; i < count_; ++i) {
		proto->add_double_data(data_vec[i]);
	}
	for(int i=0; i<count_; i++) {
		proto->add_double_mask(mask_vec[i]);
	}
	if (write_diff) {
		const double* diff_vec = cpu_diff();
		for (int i = 0; i < count_; ++i) {
			proto->add_double_diff(diff_vec[i]);
		}
	}
}

template <>
void Blob<float>::ToProto(BlobProto* proto, bool write_diff)  {
	proto->clear_shape();
	for (int i = 0; i < shape_.size(); ++i) {
		proto->mutable_shape()->add_dim(shape_[i]);
	}
	proto->clear_data();
	proto->clear_diff();
	proto->clear_mask();
	proto->clear_csrval();
	proto->clear_csrrowptr();
	proto->clear_csrcolind();



////////////////////compute csr////////////////
// if(with_csr==true&&shape_.size()==2){
	if(sparse_) {
		cusparseMatDescr_t descr;
		CUSPARSE_CHECK(cusparseCreateMatDescr(&descr));
		cusparseSetMatType(descr,CUSPARSE_MATRIX_TYPE_GENERAL);
		cusparseSetMatIndexBase(descr,CUSPARSE_INDEX_BASE_ZERO);
		cusparseDirection_t dir=CUSPARSE_DIRECTION_COLUMN;
		int lda=shape_[1];
		int M=shape_[0],N=shape_[1];
		int *nnzPerRow=0;
		LOG(INFO)<<"M N:"<<M<<" "<<N;
		LOG(INFO)<<shape_[0]<<" "<<shape_[1]<<" "<<shape_[2]<<" "<<shape_[3];
		int *nnzTotalDevHostPtr=0;
		cudaMalloc((void**)&nnzPerRow,(M)*sizeof(int));
		cudaMalloc((void**)&nnzTotalDevHostPtr,sizeof(int));
		const float* weight = gpu_data();
		CUSPARSE_CHECK(cusparseSnnz(Caffe::cusparse_handle(),dir,N,M,descr,weight,lda,nnzPerRow,nnzTotalDevHostPtr));
		cudaDeviceSynchronize();
		int* nnzTotalHostPtr=(int*)malloc(sizeof(int));
		cudaMemcpy(nnzTotalHostPtr,nnzTotalDevHostPtr,sizeof(int),cudaMemcpyDeviceToHost);
		float* csrVal=0;
		cudaMalloc((void**)&csrVal,(*nnzTotalHostPtr)*sizeof(float));
		int *csrRowPtr=0,*csrColInd=0;
		cudaMalloc((void**)&csrRowPtr,(M+1)*sizeof(int));
		cudaMalloc((void**)&csrColInd,(*nnzTotalHostPtr)*sizeof(int));
		CUSPARSE_CHECK(cusparseSdense2csc(Caffe::cusparse_handle(),N,M,descr,weight,lda,nnzPerRow,csrVal,csrColInd,csrRowPtr));

		cudaDeviceSynchronize();
		float* csrVal_cpu=(float*)malloc(sizeof(float)*(*nnzTotalHostPtr));
		LOG(INFO)<<"finish csr";
		int* csrColInd_cpu=(int*)malloc(sizeof(int)*(*nnzTotalHostPtr));
		cudaMemcpy(csrColInd_cpu,csrColInd,(*nnzTotalHostPtr)*sizeof(int),cudaMemcpyDeviceToHost);
		cudaMemcpy(csrVal_cpu,csrVal,(*nnzTotalHostPtr)*sizeof(float),cudaMemcpyDeviceToHost);
		int* csrRowPtr_cpu=(int*)malloc(sizeof(int)*(M+1));
		cudaMemcpy(csrRowPtr_cpu,csrRowPtr,(M+1)*sizeof(int),cudaMemcpyDeviceToHost);

		csrval_.reset(new SyncedMemory((*nnzTotalHostPtr)*sizeof(float)));
		float* csrval_vec=static_cast<float*>(mutable_cpu_csrval());
		setNnz(*nnzTotalHostPtr);
		for(int i=0; i<*nnzTotalHostPtr; i++) {
			csrval_vec[i]=csrVal_cpu[i];
		}
		csrrowptr_.reset(new SyncedMemory(sizeof(int)*(M+1)));
		int* csrrowptr_vec=mutable_cpu_csrrowptr();
		for(int i=0; i<M+1; i++) {
			csrrowptr_vec[i]=csrRowPtr_cpu[i];
		}
		csrcolind_.reset(new SyncedMemory(sizeof(int)*(*nnzTotalHostPtr)));
		int* csrcolind_vec=mutable_cpu_csrcolind();
		for(int i=0; i<*nnzTotalHostPtr; i++) {
			csrcolind_vec[i]=csrColInd_cpu[i];
		}

		for(int i=0; i<nnz(); i++) {
			proto->add_csrval(csrVal_cpu[i]);
			proto->add_csrcolind(csrColInd_cpu[i]);
		}

		for(int i=0; i<shape_[0]+1; i++) {
			proto->add_csrrowptr(csrRowPtr_cpu[i]);
		}
		proto->set_nnz(nnz());
		LOG(INFO)<<proto->nnz();
		free(csrVal_cpu);
		free(csrRowPtr_cpu);
		free(csrColInd_cpu);
		cusparseDestroyMatDescr(descr);
		cudaFree(csrRowPtr);
		cudaFree(csrColInd);
		cudaFree(csrVal);
		cudaFree(nnzPerRow);
		cudaFree(nnzTotalDevHostPtr);
		free(nnzTotalHostPtr);
		if(FLAGS_step!="yufeng") {
			const float* mask_vec = cpu_mask();
			for(int i=0; i< count_; i++) {
				proto->add_mask(mask_vec[i]);
			}
			const float* data_vec = cpu_data();
			for (int i = 0; i < count_; ++i) {
				proto->add_data(data_vec[i]);
			}
		}
	}
///////////////////////end////////////////////
	if(!sparse_) {
		const float* data_vec = cpu_data();
		for (int i = 0; i < count_; ++i) {
			proto->add_data(data_vec[i]);
		}
		if (write_diff) {
			const float* diff_vec = cpu_diff();
			for (int i = 0; i < count_; ++i) {
				proto->add_diff(diff_vec[i]);
			}
		}
	}
}

INSTANTIATE_CLASS(Blob);
template class Blob<int>;
template class Blob<unsigned int>;

}  // namespace caffe

