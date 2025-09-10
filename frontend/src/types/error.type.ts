/**
 * HTTP Error
 * @description HTTP errors are unexpected errors like network issues or
 * server errors (500+) that don't follow the OHT error structure
 */
export class HTTPError extends Error {
  status: number

  constructor(message: string, status: number) {
    super(message)
    this.status = status
  }
}
