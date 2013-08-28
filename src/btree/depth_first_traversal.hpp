// Copyright 2010-2012 RethinkDB, all rights reserved.
#ifndef BTREE_DEPTH_FIRST_TRAVERSAL_HPP_
#define BTREE_DEPTH_FIRST_TRAVERSAL_HPP_

#include "btree/keys.hpp"
#include "btree/slice.hpp"
#include "buffer_cache/buffer_cache.hpp"
#include "containers/archive/archive.hpp"

class superblock_t;

class counted_buf_lock_t : public buf_lock_t,
                           public single_threaded_countable_t<counted_buf_lock_t> {
public:
    template <class... Args>
    counted_buf_lock_t(Args &&... args) : buf_lock_t(std::forward<Args>(args)...) { }
};

struct dft_value_t {
    dft_value_t(const btree_key_t *key,
                const void *value,
                movable_t<counted_buf_lock_t> &&buf)
        : key_(key), value_(value), buf_(std::move(buf)) {
        guarantee(buf_.has());
    }

public:
    const btree_key_t *key() const {
        guarantee(buf_.has());
        return key_;
    }
    const void *value() const {
        guarantee(buf_.has());
        return value_;
    }

    // Releases the hold on the buf_lock_t, after which key() and value() may not be
    // used.
    void release_keepalive() { buf_.reset(); }

private:
    const btree_key_t *key_;
    const void *value_;
    movable_t<counted_buf_lock_t> buf_;
};

class depth_first_traversal_callback_t {
public:
    /* Return value of `true` indicates to keep going; `false` indicates to stop
    traversing the tree. */
    virtual bool handle_pair(dft_value_t &&value) = 0;
protected:
    virtual ~depth_first_traversal_callback_t() { }
};

enum direction_t {
    FORWARD,
    BACKWARD
};

ARCHIVE_PRIM_MAKE_RANGED_SERIALIZABLE(direction_t, int8_t, FORWARD, BACKWARD);

/* Returns `true` if we reached the end of the btree or range, and `false` if
`cb->handle_value()` returned `false`. */
bool btree_depth_first_traversal(btree_slice_t *slice, transaction_t *transaction,
        superblock_t *superblock, const key_range_t &range,
        depth_first_traversal_callback_t *cb, direction_t direction);

#endif /* BTREE_DEPTH_FIRST_TRAVERSAL_HPP_ */
