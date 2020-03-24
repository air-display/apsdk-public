#ifndef PLIST_H
#define PLIST_H
#pragma once

#include <cstdint>
#include <vector>

#define PLIST_TYPE_PRIMITIVE 0x00
#define PLIST_PRIMITIVE_TRUE 0x08
#define PLIST_PRIMITIVE_FALSE 0x09

#define PLIST_TYPE_INTEGER 0x10
#define PLIST_TYPE_REAL 0x20
#define PLIST_TYPE_DATE 0x30
#define PLIST_TYPE_DATA 0x40
#define PLIST_TYPE_STRING 0x50
#define PLIST_TYPE_UNICODE_STRING 0x60
#define PLIST_TYPE_UUID 0x80
#define PLIST_TYPE_ARRAY 0xA0
#define PLIST_TYPE_SET 0xC0
#define PLIST_TYPE_DICT 0xD0

typedef struct plist_object_s plist_object_t;

plist_object_t *plist_object_true();
plist_object_t *plist_object_false();
plist_object_t *plist_object_integer(uint64_t value);
plist_object_t *plist_object_real(double value);
plist_object_t *plist_object_data(const uint8_t *value, uint32_t valuelen);
plist_object_t *plist_object_string(const char *value);
plist_object_t *plist_object_array(uint32_t size, ...);
plist_object_t *plist_object_dict(uint32_t size, ...);

uint8_t plist_object_get_type(const plist_object_t *object);
int plist_object_primitive_get_value(const plist_object_t *object,
                                     uint8_t *value);
int plist_object_integer_get_value(const plist_object_t *object,
                                   int64_t *value);
int plist_object_real_get_value(const plist_object_t *object, double *value);
int plist_object_data_get_value(const plist_object_t *object,
                                const uint8_t **value, uint64_t *valuelen);
int plist_object_string_get_value(const plist_object_t *object,
                                  const char **value);
const plist_object_t *plist_object_array_get_value(const plist_object_t *object,
                                                   uint32_t idx);
const plist_object_t *plist_object_dict_get_value(const plist_object_t *object,
                                                  const char *key);

const plist_object_t *
plist_object_dict_get_key_value(const plist_object_t *object, const char **pkey,
                                uint32_t idx);

plist_object_t *plist_object_from_bplist(const uint8_t *data, uint32_t datalen);
int plist_object_to_bplist(plist_object_t *object, uint8_t **data,
                           uint64_t *datalen);

void plist_object_destroy(plist_object_t *object);

class auto_plist {
public:
  auto_plist() { obj_ = 0; }

  auto_plist(plist_object_t *p) { obj_ = p; }

  auto_plist &operator=(plist_object_t *p) {
    if (this->obj_) {
      plist_object_destroy(this->obj_);
      this->obj_ = 0;
    }
    this->obj_ = p;

    return *this;
  }

  auto_plist &operator=(auto_plist &other) {
    if (this->obj_) {
      plist_object_destroy(this->obj_);
      this->obj_ = 0;
    }

    this->obj_ = other.obj_;
    other.obj_ = 0;

    return *this;
  }

  bool operator!() { return (obj_ == 0); }

  operator bool() { return (obj_ != 0); }

  operator const plist_object_t *() { return obj_; }

  const plist_object_t *get() const { return obj_; }

  std::vector<uint8_t> to_bytes_array() const {
    std::vector<uint8_t> buf;
    uint8_t *data = 0;
    uint64_t length = 0;
    plist_object_to_bplist(obj_, &data, &length);
    buf.assign(data, data + length);
    free(data);
    return buf;
  }

  std::string dump() { return dump_object(obj_, 0); }

  ~auto_plist() {
    if (obj_) {
      plist_object_destroy(obj_);
      obj_ = 0;
    }
  }

protected:
  std::string dump_object(const plist_object_t *o, int indent) {
    std::ostringstream oss;
    std::string prefix_indent;

    int space_count = indent;
    while (space_count--) {
      prefix_indent += " ";
    }

    if (o) {
      uint8_t type = plist_object_get_type(o);
      switch (type) {
      case PLIST_TYPE_INTEGER: {
        int64_t v = 0;
        plist_object_integer_get_value(o, &v);
        oss << v;
      } break;
      case PLIST_TYPE_REAL: {
        double v = 0;
        plist_object_real_get_value(o, &v);
        oss << v;
      } break;
      case PLIST_TYPE_DATE: {
        oss << "Raw Data[" << 8 << "]";
      } break;
      case PLIST_TYPE_DATA: {
        const uint8_t *p = 0;
        uint64_t len = 0;
        plist_object_data_get_value(o, &p, &len);
        oss << "Raw Data[" << len << "]";
      } break;
      case PLIST_TYPE_STRING: {
        const char *v = 0;
        plist_object_string_get_value(o, &v);
        oss << v;
      } break;
      case PLIST_TYPE_UNICODE_STRING: {
        oss << "Unicode String";
      } break;
      case PLIST_TYPE_UUID: {
        oss << "UUID";
      } break;
      case PLIST_TYPE_ARRAY: {
        oss << "[" << std::endl;
        int i = 0;
        const plist_object_t *v = 0;
        while (v = plist_object_array_get_value(o, i++)) {
          oss << prefix_indent << "    " << dump_object(v, indent + 2) << ", "
              << std::endl;
        }
        oss << prefix_indent << "]";
      } break;
      case PLIST_TYPE_SET: {
        oss << "Set";
      } break;
      case PLIST_TYPE_DICT: {
        oss << "{" << std::endl;
        int i = 0;
        const char *key = 0;
        const plist_object_t *v = 0;
        while (key = 0, v = plist_object_dict_get_key_value(o, &key, i++), v) {
          oss << prefix_indent << "    " << key << ": "
              << dump_object(v, indent + 2) << ", " << std::endl;
        }
        oss << prefix_indent << "}";
      } break;
      default:
        oss << "Unknown Type";
        break;
      }
    }
    return oss.str();
  }

private:
  plist_object_t *obj_;
};

#endif
