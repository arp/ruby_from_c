module Callback
  class Handle
    def show(request)
      str = "Content-Type: text/plain\r\n\r\n"
      str << request.inspect

      return str
    end
  end
end
